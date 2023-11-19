#include <LeCode/LeCodeManager.hpp>
#include <game/System/Archive.hpp>
#include <main.hpp>
#include <core/System/RKSystem.hpp>

extern char gameID[4];

namespace LeCode
{
    LeCodeManager * LeCodeManager::sInstance = NULL;
    
    const char * LeCodeManager::GetLeCodeFilepath()
    {
        const char * filepath;
        switch (gameID[3])
        {
            case 'P':
                filepath = "/rel/lecode-PAL.bin";
                break;
            default:
            case 'E':
                filepath = "/rel/lecode-USA.bin";
                break;
            case 'J':
                filepath = "/rel/lecode-JAP.bin";
                break;
        }

        return filepath;
    }

    void LeCodeManager::LoadLeCode()
    {
        DVDFileInfo fileHandle;
        const char * filepath = GetLeCodeFilepath();

        if(DVDOpen(filepath, &fileHandle))
        {
            char buffer[0x20] __attribute__ ((aligned( 0x20 )));

            if (DVDReadPrio(&fileHandle, (void *) buffer, 0x20, 0x0, 0x2))
            {
                this->loaderHeader = (le_binary_header *) &buffer;
                DVDReadPrio(&fileHandle, this->loaderHeader->baseAddress, this->loaderHeader->totalSize, 0x0, 0x2);      

                this->loaderHeader = (le_binary_header *) this->loaderHeader->baseAddress;
                this->settingHeader = (lpar_header *)(((u8 * )this->loaderHeader) + this->loaderHeader->offsetParam);

                DVDClose(&fileHandle);      

                this->loaderHeader->entryPoint(); 

                LeCodeLoadHook::exec();
            }
            else
            {
                u32 black = 0x000000FF;
                u32 white = 0xFFFFFFFF;

                OSFatal(&white, &black, "Could not load LE-CODE files.\nMake sure your SD card is not locked.");
            }
        }
        else{     
            u32 black = 0x000000FF;
            u32 white = 0xFFFFFFFF;

            OSFatal(&white, &black, "Could not find LE-CODE files.\nMake sure your installation is correct");
        }
    } 

    LeCodeManager * LeCodeManager::GetStaticInstance()
    {
        if(sInstance == NULL)
            sInstance = new(RKSystem::mInstance.EGGSystem) LeCodeManager;
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
        address += this->settingHeader->off_cup_parameters;
        return address;
    }
    void * LeCodeManager::GetCupDefAddress()
    {
        char * address = (char*)(this->settingHeader);
        address += this->settingHeader->off_track_layout;
        return address;
    }

    void InitLoader()
    {
        LeCodeManager::GetStaticInstance()->LoadLeCode();
        return;
    }

    BootHook loadCustomCode(InitLoader, LOW);
}

