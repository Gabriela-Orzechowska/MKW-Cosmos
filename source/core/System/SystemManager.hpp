#ifndef _SYSTEM_
#define _SYSTEM_
#include "types.hpp"


class SystemManager{
public: 
    static SystemManager *sInstance; //80386000
    static void RestartGame(); //8000b1bc
    void RipFromDiscAsync(const char *path, void *heap, u32 allocDirection, u32 r7, void *buffer);
    char rippedPath[64]; //which file to rip from disc, such as ghost, savebanner.tpl etc...
    u8 unknown_0x40[0x58-0x40];
    u32 isWidePage; //0x58
    u32 gameLanguage; //0x5C
    u32 strapPageLanguage;
    u32 isProgressiveScan;
    u32 isRGB60;
    u32 unknown_0x6c[2];
    float framerate;
    float frameratePAL50;
    float frameDuration;
    bool isNdev;
    u8 unknown_0x81[0x84-0x81];
    u32 regionId;
    u8 unknown_0x88[0X98-0X88];
    const char *ndevArg;
    u32 dolStart;
    u32 dolEnd;
    u32 dolSize;
    u8 unknown_0xa8[0x10Fc-0xa8];
    u16 latitude;
    u16 longitude;
}; //total size 0x1100


#endif