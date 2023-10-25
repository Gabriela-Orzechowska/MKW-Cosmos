#include <main.hpp>
#include <game/System/Archive.hpp>
#include <game/Race/RaceData.hpp>
#include <FileManager/FileManager.hpp>

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

#ifndef NO_FAST_MENU
    //Fast Menus [east]
    kmWrite32(0x8038900C, 0x00000000);
    kmWrite32(0x80635670, 0x38000000);
    kmWrite32(0x80896830, 0x00000000);
#endif

    void CreateFolders()
    {
        DXFile::FileManager * manager = DXFile::FileManager::GetStaticInstance();
        manager->CreateFolder(packFolder);
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

}