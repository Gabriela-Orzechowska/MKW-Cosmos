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

                LeCodeLoadHook::exec();
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

    void * LeCodeManager::GetCupParamAddress()
    {
        char * address = (char*)(this->settingHeader);
        address += this->settingHeader->off_cup_par;
        return address;
    }
    void * LeCodeManager::GetCupDefAddress()
    {
        char * address = (char*)(this->settingHeader);
        address += this->settingHeader->off_cup_track;
        return address;
    }

    void InitLoader()
    {
        LeCodeManager::GetStaticInstance()->LoadLeCode();
        return;
    }

    BootHook loadCustomCode(InitLoader, HIGH);
}

