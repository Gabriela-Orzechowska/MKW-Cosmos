#ifndef _LOADER_
#define _LOADER_
#include "core/nw4r/db/Exception.hpp"
#include "core/rvl/mtx/mtx.hpp"
#include "core/rvl/vi.hpp"
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/nand.hpp>
#include <core/System/RKSystem.hpp>
#include <core/rvl/ipc/ipc.hpp>

inline int GetRegionIndex(){
   switch(*(u8*)0x80000003){
        case 'P':
            return 0;
        case 'E':
            return 1;
        case 'J':
            return 2;
        case 'K':
            return 3;
   }
   return -1;
}

struct loaderFunctions;

struct YAZHeader {
    u32 magic;
    u32 fileSize;
};

struct NETSha1Context {
    u8 unknown[0x60];
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
typedef s32 (*IOS_Open_t)(const char* deviceName, s32 mode);
typedef s32 (*IOS_Ioctlv_t)(s32 fd, u32 call, s32 in, s32 out, IOS::IOCtlvRequest* vec); 
typedef s32 (*IOS_Write_t)(s32 fd, void* buffer, u32 size);
typedef s32 (*IOS_Close_t)(s32 fd);
typedef void* (*VIGetNextFrameBuffer_t)(void);
typedef void (*DirectPrint_ChangeXfb_t)(void* buffer, s32, s32);
typedef void (*DirectPrint_DrawString_t)(s32, s32, const char* msg, s32, s32);
typedef void (*DirectPrint_StoreCache_t)();
typedef void (*NETSHA1Init_t)(NETSha1Context* context);
typedef void (*NETSHA1Update_t)(NETSha1Context* context, const void* input, u32 length);
typedef void (*NETSHA1GetDigest_t)(NETSha1Context* context, void* output);

static const u32 bufferPointer = 0x80600000;
const char nandPath[] __attribute__((aligned(0x20))) = "/title/00010001/43534D53";
const u32 devVersion = 0x44455630;

const u32 dolStart = 0x800072c0;

struct loaderFunctions {
    OSReport_t OSReport;
    OSFatal_t OSFatal;
    sprintf_t sprintf;
    RKSystem *rkSystem;
    ARCInitHandle_t ARCInitHandle;
    ARCOpen_t ARCOpen;
    SZS_Decode_t SZS_Decode;
    NANDPrivateOpen_t NANDPrivateOpen;
    NANDClose_t NANDClose;
    NANDRead_t NANDRead;
    NANDGetLength_t NANDGetLength;
    u32 dolHookAddress;
    u32 relHookAddress;
    u32 relLoadHookAddress;
};

struct DVDFunctions {
    DVDConvertPathToEntrynum_t ConvertPathToEntrynum;
    DVDFastOpen_t FastOpen;
    DVDReadPrio_t ReadPrio;
    DVDClose_t Close;
};

struct IOSFunctions {
    IOS_Open_t Open;
    IOS_Ioctlv_t IOCtlv;
    IOS_Write_t Write;
    IOS_Close_t Close;
};

struct DisplayFunctions {
    u32 endRenderAddress;
    VIGetNextFrameBuffer_t VIGetNextFrameBuffer;
    DirectPrint_ChangeXfb_t ChangeXfb;
    DirectPrint_DrawString_t DrawString;
    DirectPrint_StoreCache_t StoreCache;
};

struct SecurityFunctions {
    NETSHA1Init_t NETSHA1Init;
    NETSHA1Update_t NETSHA1Update;
    NETSHA1GetDigest_t NETSHA1GetDigest;
    u32 dolCheckEnd;
    u32 dolHash[5];
    u32 dolRiivoHash[5];
    u32 relHash[5];
};

void loadKamekBinaryFromDisc(loaderFunctions *funcs, const char *path, const char* codePath);
#endif
