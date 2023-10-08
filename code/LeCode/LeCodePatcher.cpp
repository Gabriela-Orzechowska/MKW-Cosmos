#include <LeCode/LeCodePatcher.hpp>
#include <core/rvl/os/OS.hpp>

extern char gameID[4];

char * GetLecodeFilepath();

void LoadLeCode()
{
    DVDFileInfo fileHandle;
    char * filepath = GetLecodeFilepath();

    lectHeader * header = new(lectHeader);

    if(DVDOpen(filepath, &fileHandle))
    {
        if (DVDReadPrio(&fileHandle, (void *) (0x80004000 - 0x20), 0x20, 0x0, 0x2));
        {
            header = (lectHeader *) (0x80004000 - 0x20);
            DVDReadPrio(&fileHandle, header->baseAddress, header->totalSize, 0x0, 0x2);      

            DVDClose(&fileHandle);      

            ((void (*)(void))header->entryPoint)(); 
        }
    }
    else{     
        u32 black = 0x000000FF;
        u32 white = 0xFFFFFFFF;

        OSFatal(&white, &black, "Could not load LE-CODE files.\nMake sure your installation is correct");
    }

}   

BootHook loadCustomCode(LoadLeCode, HIGH);

char filepath[20];

char * GetLecodeFilepath()
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
        snprintf(filepath, 20, LECODE_FILE, "JPN");
        break;
    }

    return filepath;
}