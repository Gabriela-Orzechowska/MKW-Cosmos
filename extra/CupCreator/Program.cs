using System;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using Veldrid;
using Veldrid.Sdl2;
using Veldrid.StartupUtilities;
using Veldrid.OpenGL;
using Veldrid.ImageSharp;
using ImPlotNET;
using ImGuiNET;
using System.Runtime.InteropServices;
using Newtonsoft.Json;

namespace CupCreator
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
    public struct OpenFileName
    {
        public int lStructSize;
        public IntPtr hwndOwner;
        public IntPtr hInstance;
        public string lpstrFilter;
        public string lpstrCustomFilter;
        public int nMaxCustFilter;
        public int nFilterIndex;
        public string lpstrFile;
        public int nMaxFile;
        public string lpstrFileTitle;
        public int nMaxFileTitle;
        public string lpstrInitialDir;
        public string lpstrTitle;
        public int Flags;
        public short nFileOffset;
        public short nFileExtension;
        public string lpstrDefExt;
        public IntPtr lCustData;
        public IntPtr lpfnHook;
        public string lpTemplateName;
        public IntPtr pvReserved;
        public int dwReserved;
        public int flagsEx;
    }

    class TrackDefinition
    {
        public string TrackName;
        public string TrackAuthor;
        public string FilePath;
        public int TrackSlot;
        public int TrackMusicSlot;
        public bool IsRetro;
        public int RetroControle;

        public TrackDefinition()
        {
            TrackName = "";
            TrackAuthor = "";
            FilePath = "";
        }
    }

    class CupDefinition
    {
        public string CupName;
        public string ImageFilePath;
        [JsonIgnore]
        public Texture image;
        public TrackDefinition[] trackDefs;

        public CupDefinition(Texture text)
        {
            CupName = "Cup Name";
            ImageFilePath = "";
            trackDefs = new TrackDefinition[4];

            trackDefs[0] = new TrackDefinition();
            trackDefs[1] = new TrackDefinition();
            trackDefs[2] = new TrackDefinition();
            trackDefs[3] = new TrackDefinition();

            image = text;
        }

    }

    class LayoutData
    {
        public int RaceCupCount;
        public int BattleCupCount;
        public bool IncludeVanilla;
        public List<CupDefinition> RaceCupDefs;
        public List<CupDefinition> BattleCupDefs;

        public LayoutData()
        {
            RaceCupCount = 0;
            BattleCupCount = 0;
            IncludeVanilla = true;
            RaceCupDefs = new List<CupDefinition>();
            BattleCupDefs = new List<CupDefinition>();
        }
    }


    class Program
    {
        private static Sdl2Window _window;
        private static GraphicsDevice _gd;
        private static CommandList _cl;
        private static ImGuiController _controller;
        private static Vector3 _clearColor = new Vector3(0.1f, 0.1f, 0.12f);

        public static string slots = "Luigi Circuit\0Moo Moo Meadows\0Mushroom Gorge\0Toad's Factory\0" +
            "Mario Circuit\0Coconut Mall\0DK Summit\0Wario's Gold Mone\0" +
            "Daisy Circuit\0Koopa Cape\0Maple Treeway\0Grumble Volcano\0" +
            "Dru Dry Ruins\0Moonview Highway\0Bowser's Castle\0Rainbow Road\0" +
            "GCN Peach Beach\0DS Yoshi Falls\0SNES Ghost Valley 2\0N64 Mario Raceway\0" +
            "N64 Sherbet Land\0GBA Shy Guy Beach\0DS Delfino Square\0GCN Waluigi Stadium\0" +
            "DS Desert Hills\0GBA Bowser Castle 3\0N64 DK's Jungle Parkway\0GCN Mario Circuit\0" +
            "SNES Mario Circuit 3\0DS Peach Gardens\0GCN DK Mountain\0N64 Bowser's Castle";

        static void Main(string[] args)
        {

            
            // Create window, GraphicsDevice, and all resources necessary for the demo.
            VeldridStartup.CreateWindowAndGraphicsDevice(
                new WindowCreateInfo(50, 50, 1600, 900, WindowState.Normal, "Cup Creator"),
                new GraphicsDeviceOptions(true, null, true, ResourceBindingModel.Improved, true, true),
                out _window,
                out _gd);
            _window.Resized += () =>
            {
                _gd.MainSwapchain.Resize((uint)_window.Width, (uint)_window.Height);
                _controller.WindowResized(_window.Width, _window.Height);
            };
            _cl = _gd.ResourceFactory.CreateCommandList();
            _controller = new ImGuiController(_gd, _gd.MainSwapchain.Framebuffer.OutputDescription, _window.Width, _window.Height);
            // _memoryEditor = new MemoryEditor();
            Random random = new Random();

            var stopwatch = Stopwatch.StartNew();
            float deltaTime = 0f;
            // Main application loop
            while (_window.Exists)
            {
                deltaTime = stopwatch.ElapsedTicks / (float)Stopwatch.Frequency;
                stopwatch.Restart();
                InputSnapshot snapshot = _window.PumpEvents();
                if (!_window.Exists) { break; }
                _controller.Update(deltaTime, snapshot); // Feed the input events to our ImGui controller, which passes them through to ImGui.

                RenderUI();

                _cl.Begin();
                _cl.SetFramebuffer(_gd.MainSwapchain.Framebuffer);
                _cl.ClearColorTarget(0, new RgbaFloat(_clearColor.X, _clearColor.Y, _clearColor.Z, 1f));
                _controller.Render(_gd, _cl);
                _cl.End();
                _gd.SubmitCommands(_cl);
                _gd.SwapBuffers(_gd.MainSwapchain);
            }

            // Clean up Veldrid resources
            _gd.WaitForIdle();
            _controller.Dispose();
            _cl.Dispose();
            _gd.Dispose();
        }

        private static void RenderUI()
        {
            ImGui.DockSpaceOverViewport(ImGui.GetMainViewport());
            RenderMainMenu();
            RenderMainWindow();
        }

        public static LayoutData layoutData = new LayoutData();

        public static Int32[] trackSlots = { 0x08, 0x01, 0x02, 0x04, 0x00, 0x05, 0x06, 0x07, 0x09, 0x0F, 0x0B, 0x03, 0x0E, 0x0A, 0x0C, 0x0D, 0x10, 0x14, 0x19, 0x1A, 0x1B, 0x1F, 0x17, 0x12, 0x15, 0x1E, 0x1D, 0x11, 0x18, 0x16, 0x13, 0x1C };
        // public static Int32[] musicSlots = {  }


        [DllImport("comdlg32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        private static extern bool GetOpenFileName(ref OpenFileName ofn);

        [DllImport("comdlg32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        private static extern bool GetSaveFileName(ref OpenFileName ofn);

        private static string ShowImageSelectDialog()
        {
            var ofn = new OpenFileName();
            ofn.lStructSize = Marshal.SizeOf(ofn);
            // Define Filter for your extensions (Excel, ...)
            ofn.lpstrFilter = "Image Files (*.png,*.jpg,*.jpeg)\0*.png;*.jpg;*jpeg\0";
            ofn.lpstrFile = new string(new char[256]);
            ofn.nMaxFile = ofn.lpstrFile.Length;
            ofn.lpstrFileTitle = new string(new char[64]);
            ofn.nMaxFileTitle = ofn.lpstrFileTitle.Length;
            ofn.lpstrTitle = "Open File Dialog...";
            if (GetOpenFileName(ref ofn))
                return ofn.lpstrFile;
            return string.Empty;
        }

        private static void RenderMainWindow()
        {
            ImGui.Begin("Editor");

            ImGui.Text("Layout Information");

            ImGui.PushItemWidth(200.0f);
            if(ImGui.InputInt("Race Cups", ref layoutData.RaceCupCount))
            {
                layoutData.RaceCupCount = Math.Clamp(layoutData.RaceCupCount, 0, 1024);

                while(layoutData.RaceCupDefs.Count < layoutData.RaceCupCount)
                {
                    var texture = _gd.ResourceFactory.CreateTexture(TextureDescription.Texture2D(256, 256, 1, 1, PixelFormat.B8_G8_R8_A8_UNorm, TextureUsage.Sampled));
                    layoutData.RaceCupDefs.Add(new CupDefinition(texture));
                }
                while(layoutData.RaceCupDefs.Count > layoutData.RaceCupCount)
                {
                    layoutData.RaceCupDefs.RemoveAt(layoutData.RaceCupDefs.Count-1);
                }

            }
            ImGui.SameLine();
            if(ImGui.InputInt("Battle Cups", ref layoutData.BattleCupCount))
            {
                layoutData.BattleCupCount = Math.Clamp(layoutData.BattleCupCount, 0, 1024);
            }
            ImGui.SameLine();
            ImGui.Checkbox("Include Vanilla Tracks", ref layoutData.IncludeVanilla);
            ImGui.PopItemWidth();

            ImGui.Separator();


            for (int i = 0; i <  layoutData.RaceCupDefs.Count; i++)
            {
                CupDefinition cupdef = layoutData.RaceCupDefs[i];

                ImGui.BeginGroup();

                var id = _controller.GetOrCreateImGuiBinding(_gd.ResourceFactory, cupdef.image);


                if (ImGui.ImageButton($"Button##{i}", id, new Vector2(128.0f, 128.0f)))
                {
                    string path = ShowImageSelectDialog();
                    if(path != string.Empty)
                    {
                        cupdef.ImageFilePath = path;
                        var img = new ImageSharpTexture(cupdef.ImageFilePath);
                        var dimg = img.CreateDeviceTexture(_gd, _gd.ResourceFactory);
                        cupdef.image = dimg;
                    }
                }
                ImGui.SameLine();
                ImGui.BeginGroup();
                ImGui.InputText($"Cup Name##{i}", ref cupdef.CupName, 256);

                ImGui.PushItemWidth(200.0f);

                {
                    ImGui.Text("Name");
                    ImGui.SameLine(207);
                    ImGui.Text("Author");
                    ImGui.SameLine(413);
                    ImGui.Text("File");
                    ImGui.SameLine(624);
                    ImGui.Text("Slot");
                    ImGui.SameLine(830);
                    ImGui.Text("Music Slot");
                    ImGui.SameLine(1036);
                    ImGui.Text("Is Retro");
                }

                for (int t = 0; t < cupdef.trackDefs.Length; t++)
                {
                    TrackDefinition trackdef = cupdef.trackDefs[t];

                    ImGui.InputText($"##name{t}{i}", ref trackdef.TrackName, 256);
                    ImGui.SameLine();
                    ImGui.InputText($"##author{t}{i}", ref trackdef.TrackAuthor, 256);
                    ImGui.SameLine();
                    ImGui.InputText($"##file{t}{i}", ref trackdef.FilePath, 256);
                    ImGui.SameLine();
                    ImGui.Combo($"##slot{t}{i}", ref trackdef.TrackSlot, slots);
                    ImGui.SameLine();
                    ImGui.Combo($"##musicslot{t}{i}", ref trackdef.TrackMusicSlot, slots);
                    ImGui.SameLine();
                    ImGui.Checkbox($"##isRetro{t}{i}", ref trackdef.IsRetro);
                }

                ImGui.PopItemWidth();
                ImGui.EndGroup();
                ImGui.EndGroup();
                ImGui.Separator();
            }


            ImGui.End();
        }

        private static void RenderMainMenu()
        {
            if (ImGui.BeginMainMenuBar())
            {
                if(ImGui.BeginMenu("File"))
                {
                    if(ImGui.Button("New Project", new Vector2(ImGui.GetWindowSize().X * 0.9f, 0.0f)))
                    {
                        if(layoutData == null)
                        {
                            layoutData = new LayoutData();
                        }
                    }
                    if(ImGui.Button("Open Project", new Vector2(ImGui.GetWindowSize().X * 0.9f, 0.0f)))
                    {
                        LoadConfigFile();
                    }
                    if(ImGui.Button("Save Project", new Vector2(ImGui.GetWindowSize().X * 0.9f, 0.0f)))
                    {
                        SaveConfigFile();
                    }
                    ImGui.Separator();
                    ImGui.Button("Export Binaries", new Vector2(ImGui.GetWindowSize().X * 0.9f, 0.0f));
                    ImGui.EndMenu();
                }
                
                ImGui.EndMainMenuBar();
            }
        }

        private static void CreateConfigFile()
        {
            

        }
        private static void SaveConfigFile()
        {
            string path = "";
            var ofn = new OpenFileName();
            ofn.lStructSize = Marshal.SizeOf(ofn);
            // Define Filter for your extensions (Excel, ...)
            ofn.lpstrFilter = "Json Files (*.json)\0*.json\0";
            ofn.lpstrFile = new string(new char[256]);
            ofn.nMaxFile = ofn.lpstrFile.Length;
            ofn.lpstrFileTitle = new string(new char[64]);
            ofn.nMaxFileTitle = ofn.lpstrFileTitle.Length;
            ofn.lpstrTitle = "Save File Dialog...";
            if (GetSaveFileName(ref ofn))
                path = ofn.lpstrFile;

            if (path == string.Empty || path == "") return;

            string output = JsonConvert.SerializeObject(layoutData);
            File.WriteAllText(path, output);
        }

        private static void LoadConfigFile()
        {
            string path = "";
            var ofn = new OpenFileName();
            ofn.lStructSize = Marshal.SizeOf(ofn);
            // Define Filter for your extensions (Excel, ...)
            ofn.lpstrFilter = "Json Files (*.json)\0*.json\0";
            ofn.lpstrFile = new string(new char[256]);
            ofn.nMaxFile = ofn.lpstrFile.Length;
            ofn.lpstrFileTitle = new string(new char[64]);
            ofn.nMaxFileTitle = ofn.lpstrFileTitle.Length;
            ofn.lpstrTitle = "Open File Dialog...";
            if (GetOpenFileName(ref ofn))
                path = ofn.lpstrFile;

            if (path == string.Empty || path == "") return; 

            string input = File.ReadAllText(path);
            layoutData = JsonConvert.DeserializeObject<LayoutData>(input);

            for(int i = 0; i < layoutData.RaceCupCount; i++)
            {
                var img = new ImageSharpTexture(layoutData.RaceCupDefs[i].ImageFilePath);
                var dimg = img.CreateDeviceTexture(_gd, _gd.ResourceFactory);
                layoutData.RaceCupDefs[i].image = dimg;
            }

        }
    }
}