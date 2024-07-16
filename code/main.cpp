#include <main.hpp>
#include "core/rvl/os/OS.hpp"
#include "kamek.hpp"
#include "vendor/lzma/7zTypes.h"
#include <core/nw4r/g3d/res/ResMat.hpp>
#include <game/Race/RaceData.hpp>
#include <FileManager/FileManager.hpp>
#include <core/System/SystemManager.hpp>
#include <include/c_stdarg.h>
#include <core/nw4r/db/Exception.hpp>
#include <Debug/IOSDolphin.hpp>
#include <Debug/SymbolMap.hpp>
#include <Debug/Draw/DebugDraw.hpp>
#include <Settings/UserData.hpp>

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
    Console_Print_t System::Console_PrintFunc = nullptr;
    Console_void_t System::Console_Clear = nullptr;

    static CosmosDebug::DebugMessage systemMessage(false, "Cosmos " __COSMOS_VERSION__ " (" __COMPILER_VERSION__ " "  __DATE__ ") Loaded");


    void System::CreateStaticInstance(){
        sInstance = new System();
        sInstance->Init();

        CosmosDebug::DebugMessage::Init();
        systemMessage.DisplayForX(15);
        System::Console_Print("Creating Cosmos System...\n");
        return;
    }
    static BootHook bhSystem(Cosmos::System::CreateStaticInstance, FIRST);

    void System::Init(){
        this->currentTTMode = COSMOS_TT_150cc;
        this->CreateFolders();
        memset(this->currentTrackHash, 0, 0x29);

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

        manager = CosmosFile::FileManager::GetNANDManager();
        s32 ret = ISFS::CreateDir(CosmosFile::nandPath, 0, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE, IOS::MODE_READ_WRITE);
        CosmosLog("Attempted creating nand folder, ret: %d\n", ret);
    }

    void System::SetTTMode(TT_MODE mode){
        this->currentTTMode = mode;
    }

    void System::Shutdown(){
        Shutdown(false);
    }

    void System::Shutdown(bool force)
    {
        if(IOS::Dolphin::IsOpen()) {
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

    asm int System::PatchRaceCount(){
        ASM(
            nofralloc;
            lis r12, sInstance@ha;
            lwz r12, sInstance@l;
            lbz r0, System.raceCount(r12);
            subi r0, r0, 1;
            blr;
        )
    }
    kmCall(0x8064f51c, System::PatchRaceCount);
    kmCall(0x806460B8, System::PatchRaceCount);

    
    void SetCC()
    {
        EngineClass cc = CC_150;
        if(System::GetStaticInstance()->GetTTMode() == COSMOS_TT_200cc) cc = CC_100;
        RaceData::GetStaticInstance()->menusScenario.GetSettings().engineClass = cc;
    }

    kmBranch(0x805e1ef4, SetCC);
    kmBranch(0x805e1d58, SetCC);

    void LoadLZMAFile(ArchiveFile * file, const char * path, EGG::Heap *heap, bool isCompressed, s32 align, EGG::Heap * fileHeap, EGG::Archive::FileInfo * fileInfo)
    {
        if(fileHeap == nullptr) fileHeap = heap;
        if(file->status == 0){
            u32 allocDirection = 2;

            void* buffer = DvdRipper::LoadToMainRam(path,nullptr,fileHeap,allocDirection,0,0,&file->archiveSize);
            if(buffer != nullptr && file->archiveSize != 0){
                file->status = 2;
                file->archiveHeap = fileHeap;
                file->rawArchive = buffer;
            }
            else {
                file->status = 0;
                file->archiveSize = 0;
            }
        }

        if(file->status == 2){
            void* dst = Compression::LZMA::Decompress((u8 *) file->rawArchive, file->archiveSize, heap);
            if(dst == nullptr){
                return;
            } else {
                fileHeap->free(file->rawArchive);
                file->archiveHeap = nullptr;
                file->archiveSize = 0;
                file->rawArchive = nullptr;
                file->decompressedArchive = dst;
                file->decompressedArchiveHeap = heap;
                file->decompressedarchiveSize = Compression::LZMA::GetSize();

                file->archive = Archive::Mount(file->decompressedArchive, heap, 4);
                file->status = 4;
            }
        }
    }

    void LoadAdditionalFiles(ArchiveFile * file, const char * path, EGG::Heap *heap, bool isCompressed, s32 align, EGG::Heap * fileHeap, EGG::Archive::FileInfo * fileInfo)
    {
        ArchiveFile* courseArchives = ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_COURSE)->archives;
        if(&courseArchives[1] == file){
            if(courseArchives[0].archive != nullptr)
                return;
        }
        if(&courseArchives[0] == file){
            LoadLZMAFile(file,path,heap,isCompressed,align,fileHeap,fileInfo);
        }
        else {
            if(&ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_UI)->archives[2] == file){
                LoadLZMAFile(file, Cosmos::UIArchive, heap, isCompressed, align, fileHeap, fileInfo);
            }
            else if(&ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_COMMON)->archives[2] == file){
                path = Cosmos::CommonArchive;
            }
            file->Load(path, heap, isCompressed, align, fileHeap, fileInfo);
        }
        CosmosLog("Loading file: %s\n", path);
    }

    kmWrite32(0x8052a108, 0x38800003); //+1 for CommonCosmos.szs
    kmWrite32(0x8052a188, 0x38800003); //+1 for UICosmos.szs
    //kmWrite32(0x8052a148, 0x38800005); //+1 for CourseDX.szs
    kmCall(0x8052aa2c, LoadAdditionalFiles);

    void PatchSuffix(ArchivesHolder* courseHolder){
        char lz[] = ".lzma";
        char szs[] = ".szs"; // if lzma one doesnt exist 
        strncpy(courseHolder->archiveSuffixes[0], lz, 6);
        strncpy(courseHolder->archiveSuffixes[1], szs, 5);
        courseHolder->sourceType[0] = ArchivesHolder::FILE_SUFFIX;
        courseHolder->sourceType[1] = ArchivesHolder::FILE_SUFFIX;
        if(courseHolder->archiveCount > 2) courseHolder->sourceType[2] = 4;
        if(courseHolder->archiveCount > 3) courseHolder->sourceType[3] = 4;
    }

    kmBranch(0x8052a21c, PatchSuffix);

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
        va_start(args, fmt); 
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

    static bool hasStartedAlready = false;
    extern "C" int fwrite(const char* ptr, u32 size, u32 nmeb, u32* stream);
    int myfwrite(const char* ptr, u32 size, u32 nmeb, u32* stream) {
        if(IOS::Dolphin::IsOpen()) return fwrite(ptr,size,nmeb,stream);
        Cosmos::Data::SettingsHolder* holder = Cosmos::Data::SettingsHolder::GetStaticInstance();
        if(holder && holder->GetSettingValue(Data::COSMOS_SETTING_LOG_TO_SD) == Data::ENABLED)
        {
            CosmosFile::FileManager* manager = CosmosFile::FileManager::GetStaticInstance();
            if(manager != nullptr) {
                manager->CreateOpen("Cosmos/Cosmos.log", CosmosFile::FILE_MODE_WRITE);
                if(!hasStartedAlready) {
                    hasStartedAlready = true;
                    manager->Write(0x10, "==============\n");
                }
                manager->Write(size * nmeb,(const void*) ptr);
                manager->Close();
            }
        }
        return fwrite(ptr, size, nmeb, stream);
    }
    kmCall(0x80011648, myfwrite);

    void LoadLoaderFuncs(){
        System::Console_PrintFunc = (Console_Print_t) *((u32*)0x80003FEC);
        System::Console_Clear = (Console_void_t) *((u32*)0x80003FE4);
        //System::Console_Clear();
    }
    kmBranch(0x80207e48, LoadLoaderFuncs);

    void ShowWelcomeMessage(){
        System::Console_Print("\n\nCosmos " __COSMOS_VERSION__ " (" __COMPILER_VERSION__ " " __DATE__ ")\n");
    }
    static BootHook bhMessage(ShowWelcomeMessage, LINK);
    
    kmWrite8(0x80022277, 0x01);
    kmWrite8(0x8002227B, 0x01);
    /*
    kmWrite32(0x80021f3c, 0x60000000);
    kmWrite32(0x80021f34, 0x60000000);
    kmWrite32(0x80021f4c, 0x60000000);
    */
}
