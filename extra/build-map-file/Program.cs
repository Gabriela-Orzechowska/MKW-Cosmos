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
        BuildKamekFile("KamekM.SMAP", "KamekM.BMAP");
        Console.WriteLine("Done.");
    }

    public static void BuildKamekFile(string input, string output)
    {
        Dictionary<int, string> symDict = new();

        string[] lines = File.ReadAllLines(input);
        int lineCount = lines.Length;

        Console.WriteLine($"{input}: Line Count: {lineCount}");
        (int left, int top) = Console.GetCursorPosition();
        for (int i = 1; i < lineCount; i++)
        {
            Console.SetCursorPosition(left, top);

            string line = lines[i];
            int Offset = Convert.ToInt32(line.Substring(0,8), 16);
            string Name = line.Substring(9);

            if(symDict.ContainsKey(Offset))
            {
                continue;
            }

            symDict.Add(Offset, Name);
            Console.WriteLine($"Processed Symbols: {i + 1}/{lineCount}");
        }

        symDict = symDict.OrderBy(obj => obj.Key).ToDictionary(obj => obj.Key, obj => obj.Value);

        string NameTableFull = "";
        int currentNameOffset = 0;

        int fileSize = 0x10 + symDict.Count * 0x8;
        MemoryStream stream = new();
        BetterBinaryWriter writer = new(stream);

        writer.Write<UInt32>(0x53594d43);
        writer.Write((UInt32)fileSize);
        writer.Write((UInt32)symDict.Count);
        writer.Write((UInt32)0x0);

        foreach(var sym in symDict)
        {
            writer.Write(sym.Key);
            writer.Write(currentNameOffset);
            NameTableFull += sym.Value;
            NameTableFull += '\0';
            currentNameOffset = NameTableFull.Length;
        }
        writer.Write(NameTableFull);
        writer.Seek(0x4);
        fileSize += NameTableFull.Length;
        writer.Write((UInt32)fileSize);
        writer.Write((UInt32)symDict.Count);
        writer.Write((UInt32)(fileSize - NameTableFull.Length));

        byte[] data = stream.ToArray();
        File.WriteAllBytes(output, data);

        Console.WriteLine($"File Size (Symbols only) {fileSize - NameTableFull.Length}");
        Console.WriteLine($"File Size (With Name Table) {fileSize}");
    }

    public static void BuildMapFile(string input, string output, bool isKamek = false)
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
    }
}