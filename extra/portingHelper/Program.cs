using System;
using System.Diagnostics;
using System.Text;

namespace portHelper
{
    public class Program
    {
        static string smapPath = ".\\GameP.SMAP";

        public static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("[ERROR] No address specified");
                return;
            }

            string[] outputs = new string[3];

            for(int a = 0; a < args.Length; a++)
            {
                var addressToPort = Convert.ToUInt32(args[a], 16);
                var addressString = args[0];

                uint size = 4;
                string[] smapLines = File.ReadAllLines(smapPath);
                foreach (var line in smapLines)
                {
                    if (line.StartsWith(addressString))
                    {
                        size = Convert.ToUInt32(line.Substring(9, 4), 16);
                    }
                }

                var stringBase = $"{addressToPort:X8}-{addressToPort + size - 1:X8}:";

                Process p = new Process();
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.FileName = "wstrt";
                p.StartInfo.Arguments = $"port {addressToPort:X8}";
                p.Start();
                string consoleOutput = p.StandardOutput.ReadToEnd();
                string[] consoleLines = consoleOutput.Split('\n');

                foreach (var line in consoleLines)
                {
                    if (line == "" || line == string.Empty) continue;
                    if (line.StartsWith('#'))
                        continue;
                    string[] values = line.Trim().Split(' ');
                    if (values[0].StartsWith('!')) continue;

                    for (int l = 1; l < 4; l++)
                    {
                        var consoleAddress = Convert.ToUInt32(values[l], 16);
                        int offset = (int)(consoleAddress - addressToPort);

                        string plus = offset < 0 ? "-" : "+";
                        int absolute = offset < 0 ? -offset : offset;

                        outputs[l - 1] += $"{stringBase} {plus}0x{absolute:X}\n";

                    }
                }
            }

            Console.WriteLine("[E]");
            Console.WriteLine(outputs[0]);
            Console.WriteLine("[J]");
            Console.WriteLine(outputs[1]);
            Console.WriteLine("[K]");
            Console.WriteLine(outputs[2]);

            return;
        }
    }
}