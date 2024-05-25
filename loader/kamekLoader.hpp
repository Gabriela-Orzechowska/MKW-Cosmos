#ifndef _LOADER_
#define _LOADER_
#include <core/rvl/dvd/dvd.hpp>
#include <core/System/RKSystem.hpp>


struct loaderFunctions;

struct YAZHeader {
    u32 magic;
    u32 fileSize;
};

typedef void (*OSReport_t) (const char *str, ...);
typedef void (*OSFatal_t) (u32 *fg, u32 *bg, const char *str, ...);
typedef int (*DVDConvertPathToEntrynum_t) (const char *path);
typedef bool (*DVDFastOpen_t) (int entrynum, DVDFileInfo *fileInfo);
typedef int (*DVDReadPrio_t) (DVDFileInfo *fileInfo, void *buffer, int length, int offset, int unk);
typedef bool (*DVDClose_t) (DVDFileInfo *fileInfo);
typedef int (*sprintf_t) (char *str, const char *format, ...);
typedef bool (*ARCInitHandle_t) (void* arcStart, ARCHandle* handle);
typedef bool (*ARCOpen_t) (ARCHandle* handle, const char* filename, ARCFileInfo* info);
typedef u32 (*SZS_Decode_t) (void* start, void* dest);

static const u32 bufferPointer = 0x80600000;


struct loaderFunctions {
    OSReport_t OSReport;
    OSFatal_t OSFatal;
    DVDConvertPathToEntrynum_t DVDConvertPathToEntrynum;
    DVDFastOpen_t DVDFastOpen;
    DVDReadPrio_t DVDReadPrio;
    DVDClose_t DVDClose;
    sprintf_t sprintf;
    RKSystem *rkSystem;
    ARCInitHandle_t ARCInitHandle;
    ARCOpen_t ARCOpen;
    SZS_Decode_t SZS_Decode;
};

void loadKamekBinaryFromDisc(loaderFunctions *funcs, const char *path, const char* codePath);
#endif