using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;
using static System.BitConverter;
using static System.Text.Encoding;

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

            public void Seek(long position) => _stream.Position = position;


            public T ReadStruct<T>(long? offset = null)
            {
                T result = default(T);

                byte[] bytes = ReadBytes(Marshal.SizeOf(typeof(T)), offset);
                if(!_isLittleEndian) FixFieldEndian(typeof(T), bytes);

                GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
                result = (T)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(T));
                handle.Free();

                return result;
            }


            private static void FixFieldEndian(Type type, byte[] data)
            {
                var fields = type.GetFields().Select(f => new{Field = f,Offset = Marshal.OffsetOf(type, f.Name).ToInt32() }).ToList();
                foreach (var field in fields) Array.Reverse(data, field.Offset, Marshal.SizeOf(field.Field.FieldType));
            }
        }
    }
}
