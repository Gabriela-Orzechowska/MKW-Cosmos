using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;
using static System.BitConverter;
using static System.Text.Encoding;
using System.Linq;
using System.Reflection;

namespace gablibela
{
    namespace io
    {

        public class BetterBinaryReader
        {
            private static Stream _stream;
            private static Encoding _encoding;
            private static bool _isLittleEndian;

            public BetterBinaryReader(Stream input) : this(input, Default, false) { }
            public BetterBinaryReader(Stream input, Encoding encoding) : this(input, encoding, false) { }

            public BetterBinaryReader(Stream input, Encoding encoding, bool littleEndian)
            {
                _stream = input;
                _encoding = encoding;
                _isLittleEndian = littleEndian;
            }

            public byte[] ReadBytes(int count, long? offset = null)
            {
                if (_stream == null) throw new FileNotFoundException();
                if (count == 0) return Array.Empty<byte>();
                long start = _stream.Position;
                if (offset != null) _stream.Position += (long)offset;
                byte[] result = new byte[count];

                int numRead = 0;
                do
                {
                    int n = _stream.Read(result, numRead, count);
                    if (n == 0)
                        break;
                    numRead += n;
                    count -= n;
                } while (count > 0);

                if (numRead != result.Length)
                {
                    byte[] copy = new byte[numRead];
                    Buffer.BlockCopy(result, 0, copy, 0, numRead);
                    result = copy;
                }
                if (offset != null) _stream.Position = start;
                return result;

            }
            public string ReadString(ushort lenght, long? offset = null) => _encoding.GetString(ReadBytes(lenght, offset));

            public string ReadStringNT(long? offset = null)
            {
                if (_stream == null) throw new FileNotFoundException();
                long start = _stream.Position;
                if (offset != null) _stream.Position += (long)offset;
                List<byte> strBytes = new List<byte>();
                int b;
                while ((b = _stream.ReadByte()) != 0)
                {
                    strBytes.Add((byte)b);
                }
                string output = _encoding.GetString(strBytes.ToArray());
                if (output.EndsWith('\0')) output.Substring(0, output.Length - 1);
                if (offset != null) _stream.Position = start;
                return output;
            }

            public ushort ReadUInt8(long? offset = null) => ToUInt16(Flip(ReadBytes(1, offset)), 0);

            public ushort ReadUInt16(long? offset = null) => ToUInt16(Flip(ReadBytes(2, offset)), 0);

            public uint ReadUInt32(long? offset = null) => ToUInt32(Flip(ReadBytes(4, offset)), 0);

            public short ReadInt8(long? offset = null) => ReadBytes(1, offset)[0];

            public short ReadInt16(long? offset = null) => ToInt16(Flip(ReadBytes(2, offset)), 0);

            public int ReadInt32(long? offset = null) => ToInt32(Flip(ReadBytes(4, offset)), 0);

            public float ReadFloat(long? offset = null) => ToSingle(Flip(ReadBytes(4, offset)), 0);

            public T ReadEnum<T>(long? offset = null) where T : Enum => (T)(object)(int)ReadByte(offset);

            public byte ReadByte(long? offset = null) => ReadBytes(1, offset)[0];

            private static byte[] Flip(byte[] value)
            {
                Array.Reverse(value);
                return value;
            }
            public long Length() => _stream.Length;

            public long Position() => _stream.Position;

            public long Tell() => _stream.Position;

            public void Seek(long position) => _stream.Position = position;

            public void PushStream(long offset) => _stream.Position += offset;

            public void Align(int value) => _stream.Position = (_stream.Position + (value - 1)) & ~(value - 1);

            public T[] ReadArray<T>(int size, long? offset = null) where T : IComparable<T>
            {
                List<T> list = new List<T>();
                int typeSize = Marshal.SizeOf(typeof(T));
                for (int i = 0; i < size; i++)
                {
                    if (offset != null)
                        offset += typeSize * i;
                    T value = ReadType<T>(offset);
                    list.Add(value);

                }
                return list.ToArray();
            }

            public T ReadType<T>(long? offset = null) where T : IComparable<T>
            {
                T result = default(T);
                Type type = typeof(T);

                switch (type.Name.ToLower())
                {
                    case "int16":
                        result = (T)(object)ReadInt16(offset); break;
                    case "int32":
                        result = (T)(object)ReadInt32(offset); break;
                    case "uint16":
                        result = (T)(object)ReadUInt16(offset); break;
                    case "uint32":
                        result = (T)(object)ReadUInt32(offset); break;
                    case "sbyte":
                    case "byte":
                        result = (T)(object)ReadByte(offset); break;
                    case "float":
                        result = (T)(object)ReadFloat(offset); break;
                }
                return result;
            }

            public T ReadStruct<T>(long? offset = null)
            {
                var bytes = ReadBytes(Marshal.SizeOf(typeof(T)), offset);
                if(!_isLittleEndian) FixFieldEndian(typeof(T), ref bytes);

                GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
                T result = (T)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(T));
                handle.Free();

                return result;
            }

            private static bool isStruct(Type type)
            {
                return type.IsValueType && !type.IsPrimitive;
            }

            private static void FixFieldEndian(Type type, ref byte[] data, int classOffset = 0)
            {
                var fields = type.GetFields().Select(f => new{Field = f,Offset = Marshal.OffsetOf(type, f.Name).ToInt32() }).ToList();
                foreach (var field in fields)
                {
                    var fieldType = field.Field.FieldType;
                    Int32 length = 0;
                    if (fieldType.IsEnum)
                    {
                        length = Marshal.SizeOf(Enum.GetUnderlyingType(field.Field.FieldType));
                    }
                    else if (fieldType.IsArray)
                    {
                        var attributes = field.Field.GetCustomAttributes(typeof(MarshalAsAttribute), false);
                        MarshalAsAttribute marshal = (MarshalAsAttribute)attributes[0];
                        var arrayLenght = marshal.SizeConst;

                        var elementType = fieldType.GetElementType();

                        if (elementType.IsEnum)
                            length = Marshal.SizeOf(Enum.GetUnderlyingType(elementType));
                        else
                            length = Marshal.SizeOf(elementType);
                        for (var i = 0; i < arrayLenght; i++)
                        {
                            if (isStruct(elementType))
                                FixFieldEndian(elementType, ref data, field.Offset + classOffset + (i * length));
                            else
                                Array.Reverse(data, field.Offset + classOffset + (i * length), length);
                        }
                        continue;
                    }
                    else if (isStruct(fieldType) || fieldType.IsClass)
                    {
                        FixFieldEndian(fieldType, ref data, field.Offset + classOffset);
                        continue;
                    }
                    else
                    {
                        length = Marshal.SizeOf(fieldType);
                    }

                    Array.Reverse(data, field.Offset + classOffset, length);
                }
            }
        }
    }
}
