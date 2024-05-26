#ifndef _LOADER_
#define _LOADER_
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/nand.hpp>
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
typedef s32 (*NANDPrivateOpen_t) (const char* path, NANDFileInfo* info, u8 type);
typedef s32 (*NANDClose_t) (NANDFileInfo* info);
typedef s32 (*NANDRead_t) (NANDFileInfo* info, void* buffer, u32 length);
typedef s32 (*NANDGetLength_t) (NANDFileInfo* info, u32 *length);

static const u32 bufferPointer = 0x80600000;
const char nandPath[] __attribute__((aligned(0x20))) = "/title/00010001/43534D53";

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
    NANDPrivateOpen_t NANDPrivateOpen;
    NANDClose_t NANDClose;
    NANDRead_t NANDRead;
    NANDGetLength_t NANDGetLength;
};

void loadKamekBinaryFromDisc(loaderFunctions *funcs, const char *path, const char* codePath);
#endif