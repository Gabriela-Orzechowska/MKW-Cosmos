#include <LeCode/LeCodeManager.hpp>
#include <game/System/Archive.hpp>

extern char gameID[4];
char filepath[20];

namespace LeCode
{
    LeCodeManager * LeCodeManager::sInstance = nullptr;
    
    char * LeCodeManager::GetLeCodeFilepath()
    {
        switch (gameID[3])
        {
            default:
            case 'P':
                snprintf(filepath, 20, LECODE_FILE, "PAL");
                break;
            case 'E':
                snprintf(filepath, 20, LECODE_FILE, "USA");
                break;
            case 'J':
                snprintf(filepath, 20, LECODE_FILE, "JAP");
                break;
        }

        return filepath;
    }

    void * LoadAdditionalBinaries(ArchiveRoot * archive, ArchiveSource source, const char * name)
    {
        void * file = archive->GetFile(ARCHIVE_HOLDER_COURSE, name, 0);
        if(file == NULL)
        {
            char commonName[0x30];
            snprintf(commonName, 0x30, "Common/%s", name);
            file = archive->GetFile(ARCHIVE_HOLDER_COURSE, commonName, 0);
        }
        if(file == NULL)
            file = archive->GetFile(ARCHIVE_HOLDER_COMMON, name, 0);
        return file;
    }

    kmCall(0x8082c140, LoadAdditionalBinaries);
    kmCall(0x807f92ac, LoadAdditionalBinaries);

    void LeCodeManager::LoadLeCode()
    {
        DVDFileInfo fileHandle;
        char * filepath = GetLeCodeFilepath();

        if(DVDOpen(filepath, &fileHandle))
        {
            if (DVDReadPrio(&fileHandle, (void *) (0x80004000 - 0x20), 0x20, 0x0, 0x2));
            {
                this->loaderHeader = (le_binary_header *) (0x80004000 - 0x20);
                DVDReadPrio(&fileHandle, this->loaderHeader->baseAddress, this->loaderHeader->totalSize, 0x0, 0x2);      

                this->loaderHeader = (le_binary_header *) this->loaderHeader->baseAddress;
                this->settingHeader = (lpar_header *)(((u8 * )this->loaderHeader) + 0x40);

                DVDClose(&fileHandle);      

                ((void (*)(void))this->loaderHeader->entryPoint)(); 

                DX::CreateCall(0x8082c140, (u32)&LoadAdditionalBinaries);
                DX::CreateCall(0x807f92ac, (u32)&LoadAdditionalBinaries);
            }
        }
        else{     
            u32 black = 0x000000FF;
            u32 white = 0xFFFFFFFF;

            OSFatal(&white, &black, "Could not load LE-CODE files.\nMake sure your installation is correct");
        }
    } 

    LeCodeManager * LeCodeManager::GetStaticInstance()
    {
        if(sInstance == nullptr)
            sInstance = new(LeCodeManager);
        return sInstance;
    }

    void * LeCodeManager::GetLoaderAddress()
    {
        return this->loaderHeader;
    }

    u32 LeCodeManager::GetTrackID()
    {
        char * address = (char*)(this->settingHeader);
        address += this->settingHeader->off_eod;
        return *(u32*)(address);
    }

    void InitLoader()
    {
        LeCodeManager::GetStaticInstance()->LoadLeCode();
        return;
    }

    BootHook loadCustomCode(InitLoader, HIGH);
}

