#include <main.hpp>
#include <game/System/Archive.hpp>
#include <core/nw4r/g3d/res/ResMat.hpp>
#include <game/Race/RaceData.hpp>
#include <FileManager/FileManager.hpp>
#include <core/System/SystemManager.hpp>
#include <include/c_stdarg.h>
#include <core/nw4r/db/Exception.hpp>
#include <Debug/IOSDolphin.hpp>
#include <Debug/SymbolMap.hpp>

extern char gameID[4];

namespace Cosmos{

    //Wiimmfi Stuff
    using IOS::Open2ndInst;
    asm IOS::IPCResult Open(char *path, IOS::Mode mode){ 
        ASM(
            nofralloc;
            stwu sp, -0x0020 (sp);
            b Open2ndInst;
        )
    }

    System * System::sInstance = nullptr;

    void System::CreateStaticInstance(){
        sInstance = new System();
        sInstance->Init();
        return;
    }
    static BootHook bhSystem(Cosmos::System::CreateStaticInstance, FIRST);

    void System::Init(){
        this->currentTTMode = COSMOS_TT_150cc;
        this->CreateFolders();

        EGG::ProcessMeter * meter = RKSystem::mInstance.processMeter;

        meter->xOrigin = 20.0f;
        meter->yOrigin = 1.0f;
        meter->xSize = 50.0f;
        meter->ySize = 0.75f;

    }

    void System::CreateFolders()
    {
        CosmosFile::FileManager * manager = CosmosFile::FileManager::GetStaticInstance();
        manager->CreateFolder(packFolder);
        manager->CreateFolder(ghostFolder);

        s32 ret = IOS::Dolphin::Open();
        if(ret > 0)
        {
            isDolphin = true;
            IOS::Dolphin::Close();
        }
         
    }

    void System::SetTTMode(TT_MODE mode){
        this->currentTTMode = mode;
        CosmosLog("Setting TT gamemode to: %s\n", mode == COSMOS_TT_200cc ? "200cc" : "150cc");
    }

    void System::Shutdown(){
        Shutdown(false);
    }

    void System::Shutdown(bool force)
    {
        if(isDolphin) {
            if(force) OSShutdownToSBY();
            else OSShutdownSystem();
        }
        else 
        {
            s32 ret;
            ret = Cosmos::Open("/title/00010001/52494956/content/title.tmd\0", IOS::MODE_NONE); 
            if(ret >= 0){
                ISFS::Close(ret);
                OSLaunchTitle(0x00010001, 0x52494956);
            }
            ret = Cosmos::Open("/title/00010001/4c554c5a/content/title.tmd\0", IOS::MODE_NONE);
            if(ret >= 0){
                ISFS::Close(ret);
                OSLaunchTitle(0x00010001, 0x4c554c5a);
            }
            ret = Cosmos::Open("/title/00010001/48424330/content/title.tmd\0", IOS::MODE_NONE); 
            if(ret >= 0){
                ISFS::Close(ret);
                OSLaunchTitle(0x00010001, 0x48424330);
            }
            SystemManager::sInstance->GoToWiiMenu();
        }
    }

    void System::Restart()
    {
        SystemManager::sInstance->RestartGame();
    }
    void System::HardRestart()
    {
        OSRestart(0);
    }

    void SetCC()
    {
        EngineClass cc = CC_100;
        if(System::GetStaticInstance()->GetTTMode() == COSMOS_TT_200cc) cc = CC_150;
        RaceData::sInstance->menusScenario.settings.engineClass = cc;
    }

    kmBranch(0x805e1ef4, SetCC);
    kmBranch(0x805e1d58, SetCC);


    void LoadAdditionalFiles(ArchiveFile * file, const char * path, EGG::Heap *heap, bool isCompressed, s32 align, EGG::Heap * fileHeap, EGG::Archive::FileInfo * fileInfo)
    {
        if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_UI]->archives[2] == file){
            path = Cosmos::UIArchive;
        }
        else if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COMMON]->archives[2] == file){
            path = Cosmos::CommonArchive;
        }
        //else if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COURSE]->archives[4] == file){
        //    path = DX::CourseArchive;
        //}
        file->Load(path, heap, isCompressed, align, fileHeap, fileInfo);
        CosmosLog("Loading %s\n", path);
    }

    kmWrite32(0x8052a108, 0x38800003); //+1 for CommonCosmos.szs
    kmWrite32(0x8052a188, 0x38800003); //+1 for UICosmos.szs
    //kmWrite32(0x8052a148, 0x38800005); //+1 for CourseDX.szs
    kmCall(0x8052aa2c, LoadAdditionalFiles);

    //Unlock Everything Without Save [_tZ]
    kmWrite32(0x80549974,0x38600001);

    void CreateBranch(u32 from, void * to)
    {
        u32 offset = ((u32)to)-from;
        u32 command = 0x48000000 | (offset & 0x03FFFFFF);
        *((u32 *)from) = command;
    }
    void CreateCall(u32 from, void * to)
    {
        u32 offset = ((u32)to)-from+1;

        u32 command = 0x48000000 | (offset & 0x03FFFFFF);
        *((u32 *)from) = command;
    }
    
    u32 GetPortAddress(u32 PAL, u32 NTSCU, u32 NTSCJ, u32 NTSCK)
    {
        switch (gameID[3])
        {
            default:
            case 'P':
                return PAL;
            case 'E':
                return NTSCU;
            case 'J':
                return NTSCJ;
            case 'K':
                return NTSCK;
        }
        return PAL;
    }


//38600001
//kmWrite32(0x80007c60, 0x4e800020);

//#define FORCEOSFATAL

    void Panic(char * file, int line, char *fmt, ...)
    {
        OSContext * context = OSGetCurrentContext();
        
        char output[0x130];
        char output2[0x130];
        va_list args;
        #ifndef __INTELLISENSE__ // I dont like errors
        va_start(args, fmt); 
        #endif
        vsnprintf(output2, 0x100, fmt, args);
        va_end(args);
        

        snprintf(output, 0x130, "%s:%d: %s\n", file, line, output2);

    
        char stack[0x400] = "\nSTACK TRACE:\n";
        u32 * stackRegister = OSGetStackPointer();
        for(int i = 0; i < 10; i++)
        {
            if(stackRegister == nullptr || stackRegister == (u32 *) -1) break;
            char buffer[0x80];
            snprintf(buffer, 0x80, "%08X: %08X %08X: %s\n", stackRegister, *stackRegister, stackRegister[0x1], CosmosDebug::SymbolManager::GetSymbolName(stackRegister[0x1]));
            strcat(stack,buffer);
            stackRegister = (u32 *) *stackRegister;
        }
  
        strcat(output, stack);
  
        nw4r::db::ExceptionCallbackParam exc;
        exc.error = 0x32; 
        exc.context = context;
        exc.dar = (u32) output;
        exc.dsisr = 0x0;

        #ifndef FORCEOSFATAL
        nw4r::db::DumpException_(&exc);
        #else
        char final[0x300];
        snprintf(final, 0x140, "%s\n\n%s", "Cosmos::Panic() has been called", output);
        u32 black = 0x000000FF;
        u32 white = 0xc2659dFF;

        OSFatal(&white, &black, final);
        #endif
    }
}