#ifndef _NW4R_DB_EXCEPTION_
#define _NW4R_DB_EXCEPTION_
#include "types.hpp"
#include <core/rvl/os/thread.hpp>
#include <core/nw4r/db/Console.hpp>

namespace nw4r{
namespace db {


struct GXRenderModeObj {
    int tv_mode;
    u16 fb_width;
    u16 efb_height;
    u16 xfb_height;
    u16 vi_x;
    u16 vi_y;
    u16 vi_width;
    u16 vi_height;
    int vi_xfb;
    u8 field;
    u8 aa;
    u8 sample[12][2];
    u8 vert_filter[7];
};

extern GXRenderModeObj* mainConsole;

typedef bool (*ExceptionUserCallback)(nw4r::db::detail::ConsoleHandle consoleHandle, void *arg);

struct ExceptionInfo {
    OSThread osThread;  //0
    OSMessageQueue osMessageQueue; //0x318
    void *framebuffer; //0x338
    u32 sp;
    nw4r::db::detail::ConsoleHandle consoleHandle;
    const nw4r::db::GXRenderModeObj *gxRenderModeObj;
    nw4r::db::ExceptionUserCallback exceptionUserCallback;
    void *exceptionUserCallbackArg;
    u32 msr;
    u32 fpscr;
    u16 displayInfo;
    u8 _35A[0x360 - 0x35A];
};
size_assert(ExceptionInfo, 0x360);

enum {
    EXCEPTION_INFO_NONE = (0 << 0),
    EXCEPTION_INFO_MAIN = (1 << 0),
    EXCEPTION_INFO_GPR = (1 << 1),
    EXCEPTION_INFO_GPRMAP = (1 << 2),
    EXCEPTION_INFO_SRR0MAP = (1 << 3),
    EXCEPTION_INFO_FPR = (1 << 4),
    EXCEPTION_INFO_TRACE = (1 << 5),

    EXCPETION_INFO_DEFAULT = EXCEPTION_INFO_MAIN | EXCEPTION_INFO_TRACE,
    EXCEPTION_INFO_ALL = EXCEPTION_INFO_MAIN | EXCEPTION_INFO_GPR | EXCEPTION_INFO_GPRMAP |
            EXCEPTION_INFO_SRR0MAP | EXCEPTION_INFO_FPR | EXCEPTION_INFO_TRACE
};

struct Exception{
	static Exception sInstance; //802a70b8
	static void Init(); //800023150
    /*
    OSThread osThread;
    OSMessageQueue messaheQueue;
    void * frameBuffer;
    u32 sp;
    nw4r::db::detail::ConsoleHead consoleHead;
    const GXRenderModeObj *gxRenderModeObj;
    void * userCallback;
    void * userCallbackParams;
    u32 msr;
    u32 fpscr;
    u16 displayInfo;
    */
	u8 unknown[0x360]; 
}; //total size 0x360
static_assert(sizeof(Exception) == 0x360, "Exception");


typedef struct ExceptionCallbackParam{
    u16 error;
    u8 padding[2];
    OSContext* context;
    u32 dsisr;
    u32 dar;
} ExceptionCallbackParam;

extern char * ExceptionNameTable[16]; //80271cac

// prints message to NW4R console
void DirectPrint_ChangeXfb(void *frameBuffer, u16 width, u16 height);
void DirectPrint_DrawString(u32 xCoord, u32 yCoord, bool hasWrapping, const char*string);
void DirectPrint_StoreCache();
void Exception_Printf_(const char *, ...);
void PrintContext_(u16 error, const OSContext *context, u32 dsisr, u32 dar);
void DumpException_(const ExceptionCallbackParam *param);
void ShowMainInfo_(u32 error, const OSContext *context, u32 dsisr, u32 dar); //80023980
void ShowFloat_(const OSContext * context); //80023c70
}//namespace db
}//namespace nw4r
#endif