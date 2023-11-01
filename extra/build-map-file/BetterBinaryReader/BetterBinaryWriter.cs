using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.Contracts;
using System.IO;
using System.Text;
using static System.BitConverter;
using static System.Text.Encoding;

namespace gablibela
{
    namespace io
    {
        public class BetterBinaryWriter
        {
            private static Stream _stream;
            private static Encoding _encoding;

            public BetterBinaryWriter(Stream input) : this(input, Default)
            {
            }
            public BetterBinaryWriter(Stream input, Encoding encoding)
            {
                _stream = input;
                _encoding = encoding;
            }

            public void Write(byte[] buffer, ulong? offset = null, int index = 0, int? count = null)
            {
                if (buffer == null)
                    throw new ArgumentNullException("buffer");
                if (count == null)
                    count = buffer.Length;

                long start = _stream.Position;
                if (offset != null) _stream.Position += (long)offset;

                Contract.EndContractBlock();
                _stream.Write(buffer, index, (int)count);
                if (offset != null) _stream.Position = start;

            }

            public void Write<T>(T value, ulong? offset = null) where T : IComparable<T>
            {
                object _val = value;
                switch (value.GetType().Name.ToLower())
                {
                    case "int16":
                        Write((short)_val, offset); break;
                    case "int32":
                        Write((int)_val, offset); break;
                    case "int64":
                        Write((long)_val, offset); break;
                    case "uint16":
                        Write((ushort)_val, offset); break;
                    case "uint32":
                        Write((uint)_val, offset); break;
                    case "uint64":
                        Write((ulong)_val, offset); break;
                    case "byte":
                        Write((byte)_val, offset); break;
                    case "float":
                        Write((float)_val, offset); break;
                    case "string":
                        Write((string)_val, offset); break;

                }

            }

            public void Write(byte value, ulong? offset = null)
            {
                byte[] _buffer = new byte[1] { value };
                Write(_buffer, offset);
            }

            public void Write(short value, ulong? offset = null)
            {
                byte[] _buffer = Flip(GetBytes(value));
                Write(_buffer, offset);
            }

            public void Write(ushort value, ulong? offset = null)
            {
                byte[] _buffer = Flip(GetBytes(value));
                Write(_buffer, offset);
            }

            public void Write(int value, ulong? offset = null)
            {
                byte[] _buffer = Flip(GetBytes(value));
                Write(_buffer, offset);
            }

            public void Write(uint value, ulong? offset = null)
            {
                byte[] _buffer = Flip(GetBytes(value));
                Write(_buffer, offset);
            }

            public void Write(long value, ulong? offset = null)
            {
                byte[] _buffer = Flip(GetBytes(value));
                Write(_buffer, offset);
            }

            public void Write(ulong value, ulong? offset = null)
            {
                byte[] _buffer = Flip(GetBytes(value));
                Write(_buffer, offset);
            }

            public void Write(float value, ulong? offset = null)
            {
                byte[] _buffer = Flip(GetBytes(value));
                Write(_buffer, offset);
            }

            public void Write(string value, ulong? offset = null)
            {
                byte[] _buffer = _encoding.GetBytes(value);
                Write(_buffer, offset);
            }



            public void CloseStream() => _stream.Close();

            private static byte[] Flip(byte[] value)
            {
                Array.Reverse(value);
                return value;
            }


            public long Length() => _stream.Length;

            public long Position() => _stream.Position;

            public void Seek(long offset) => _stream.Position= offset;

        }
    }
}
