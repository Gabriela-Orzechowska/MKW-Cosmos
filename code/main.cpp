#include <main.hpp>
#include <game/System/Archive.hpp>
#include <game/Race/RaceData.hpp>
#include <FileManager/FileManager.hpp>
#include <core/System/SystemManager.hpp>
#include <include/c_stdarg.h>
#include <core/nw4r/db/Exception.hpp>
#include <Debug/IOSDolphin.hpp>

extern char gameID[4];

namespace DX{

    //Wiimmfi Stuff
    using IOS::Open2ndInst;
    asm s32 Open(char *path, IOS::Mode mode){ 
        ASM(
            nofralloc;
            stwu sp, -0x0020 (sp);
            b Open2ndInst;
        )
    }

    void SetTTCC(TT_MODE ttMode)
    {
        TTMode = ttMode;
        SetCC();
    }

    void SetCC()
    {
        EngineClass cc = CC_100;
        if(TTMode == DX_TT_200CC) cc = CC_150;
        RaceData::sInstance->menusScenario.settings.engineClass = cc;
    }

    TT_MODE GetTTMode()
    {
        return TTMode;
    }

    kmBranch(0x805e1ef4, SetCC);
    kmBranch(0x805e1d58, SetCC);

    void LoadAdditionalFiles(ArchiveFile * file, const char * path, EGG::Heap *heap, bool isCompressed, s32 align, EGG::Heap * fileHeap, EGG::Archive::FileInfo * fileInfo)
    {
        if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_UI]->archives[2] == file){
            path = DX::UIArchive;
        }
        else if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COMMON]->archives[2] == file){
            path = DX::CommonArchive;
        }
        //else if(&ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_COURSE]->archives[4] == file){
        //    path = DX::CourseArchive;
        //}
        file->Load(path, heap, isCompressed, align, fileHeap, fileInfo);
        OSReport("[DX] Loading %s\n", path);
    }

    kmWrite32(0x8052a108, 0x38800003); //+1 for CommonDX.szs
    kmWrite32(0x8052a188, 0x38800003); //+1 for UIDX.szs
    //kmWrite32(0x8052a148, 0x38800005); //+1 for CourseDX.szs
    kmCall(0x8052aa2c, LoadAdditionalFiles);

    //Unlock Everything Without Save [_tZ]
    kmWrite32(0x80549974,0x38600001);

    void CreateFolders()
    {
        DXFile::FileManager * manager = DXFile::FileManager::GetStaticInstance();
        manager->CreateFolder(packFolder);
        manager->CreateFolder(ghostFolder);

        s32 ret = IOS::Dolphin::Open();
        if(ret > 0)
        {
            isDolphin = true;
            IOS::Dolphin::Close();
        }
                
    }

    BootHook InitFolders(CreateFolders, HIGH);

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

    void Shutdown()
    {
        Shutdown(false);
    }

    void Shutdown(bool force)
    {
        if(isDolphin) {
            if(force) OSShutdownToSBY();
            else OSShutdownSystem();
        }
        else SystemManager::sInstance->GoToWiiMenu();
    }

    void Restart()
    {
        SystemManager::sInstance->RestartGame();
        
    }

//38600001
//kmWrite32(0x80007c60, 0x4e800020);

#define FORCEOSFATAL

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

        char stack[0x200] = "\n STACK TRACE:\n";
        u32 * stackRegister = OSGetStackPointer();
        for(int i = 0; i < 10; i++)
        {
            if(stackRegister == nullptr || stackRegister == (u32 *) -1) break;
            char buffer[0x28];
            snprintf(buffer, 0x20, "%08x: %08x %08x\n", stackRegister, *stackRegister, stackRegister[0x1]);
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
        snprintf(final, 0x140, "%s\n\n%s", "DX::Panic() has been called", output);
        u32 black = 0x000000FF;
        u32 white = 0xc2659dFF;

        OSFatal(&white, &black, final);
        #endif

    }

}