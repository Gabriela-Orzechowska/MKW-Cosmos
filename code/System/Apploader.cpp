#include <kamek.hpp>
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/os/OS.hpp>

typedef void (entryPoint)();

extern void Run(entryPoint entry);

#define LOADER_SIZE 0xC20

void PreRestart(u32 mode)
{
    void * buffer = (void *) 0x81400000;
    DVDFileInfo fileHandle;
    u32 black = 0;
    u32 white = ~1;
    if(DVDOpen("/rel/Loader.bin", &fileHandle))
    {
        if (DVDReadPrio(&fileHandle, (void *) buffer, LOADER_SIZE, 0x0, 0x2))
        {
            DVDClose(&fileHandle);
        }
        else OSFatal(&black, &white, "Failed to read a file");
    }
    else OSFatal(&black, &white, "Failed to open a file");
    OSRestart(mode);
}

kmCall(0x8000b2c4, PreRestart);
kmCall(0x8000b1e0, PreRestart);
kmCall(0x8000b4b4, PreRestart);

void PreRun(entryPoint point)
{
    *((u32 *)0x81334080) = 0x480CCAF0;

    Run(point);
}

kmCall(0x801a4498, PreRun);
kmCall(0x801a4628, PreRun);
