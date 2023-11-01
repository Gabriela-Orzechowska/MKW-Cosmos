using System;
using System.IO;
using gablibela.io;

public class Program
{
    struct Header
    {
        public UInt32 magic;
        public UInt32 size;
        public UInt32 symbolCount;
        public UInt32 nameTableOffset;
    }

    struct Symbol
    {
        public UInt32 data1;
        public UInt32 data2;
    }

    public static Dictionary<UInt32, string> NameDict = new();

    public static void Main(string[] args)
    {
        BuildMapFile("GameP.SMAP", "GameP.BMAP");
    }

    public static void BuildMapFile(string input, string output)
    {
        string NameTableFull = "";
        List<Symbol> symbols = new();   

        string[] lines = File.ReadAllLines(input);
        int lineCount = lines.Length;

        int currentNameOffset = 0;

        Console.WriteLine($"{input}: Line Count: {lineCount}");
        (int left, int top) = Console.GetCursorPosition();


        for(int i = 0; i < lineCount; i++)
        {
            Console.SetCursorPosition(left, top);

            var line = lines[i];
            UInt32 address = Convert.ToUInt32(line.Substring(0, 8), 16);
            UInt16 size = Convert.ToUInt16(line.Substring(9, 4), 16);
            string name = line.Substring(14);

            NameTableFull += name;
            NameTableFull += "\0";

            
            Symbol symbol = new();
            symbol.data1 = (UInt32)((address & 0x00FFFFFF) << 8) | (((uint)size & 0xFF00) >> 8);
            symbol.data2 = (UInt32)((currentNameOffset & 0x00FFFFFF)) | (((uint)size & 0x00FF) << 24);
            //symbol.data = (ulong)((address << 40) & 0xFFFFFF0000000000) | (ulong)((size << 24) | (currentNameOffset & 0x00FFFFFF));
            //symbol.sizeNameOffset = (uint)((size << 24) | (currentNameOffset & 0x00FFFFFF));

            currentNameOffset = NameTableFull.Length;
            symbols.Add(symbol);
            Console.WriteLine($"Processed Symbols: {i + 1}/{lineCount}");
        }



        MemoryStream stream = new();
        BetterBinaryWriter writer = new(stream);

        // Write Header
        int fileSize = 0x10 + symbols.Count * 8 + NameTableFull.Length;
        int tableOffset = 0x10 + symbols.Count * 8;

        Console.WriteLine($"File Size (Symbols only) {tableOffset}");
        Console.WriteLine($"File Size (With Name Table) {fileSize}");

        writer.Write<UInt32>(0x53594d42);
        writer.Write((UInt32)fileSize);
        writer.Write((UInt32)symbols.Count);
        writer.Write((UInt32)tableOffset);

        for(int i = 0; i < symbols.Count;i++)
        {
            Symbol symbol = symbols[i];
            writer.Write((UInt32)symbol.data1);
            writer.Write((UInt32)symbol.data2);
        }
        writer.Write(NameTableFull);

        byte[] data = stream.ToArray();
        File.WriteAllBytes(output, data);

        Console.WriteLine("Done");
    }
}