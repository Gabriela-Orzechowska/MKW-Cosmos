#include <kamek.hpp>
#include <core/egg/Exception.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/base/ppc.hpp>
#include <core/rvl/devfs/isfs.hpp>
#include <core/rvl/os/OS.hpp>
#include <core/rvl/os/Error.hpp>
#include <core/rvl/os/thread.hpp>
#include <core/egg/Exception.hpp>
#include <core/nw4r/db/Exception.hpp>
#include <include/c_stdarg.h>

extern char gameID[4];

namespace DXDebug{

    static char output[1024];

    void HandlePanic( const char *file, int line, const char *fmt, va_list vlist, bool halt, u32 LR)
    {
        char format[1024];
        snprintf(output, 1024, "%s:%d Panic:\n", file, line);
        vsnprintf(format, 1024, fmt, vlist);
        snprintf(output, 1024, "%s%s", output, format);
        OSContext * context = OSGetCurrentContext();
        nw4r::db::ExceptionCallbackParam exc;
        exc.error = 0x30; 
        exc.context = context;
        exc.dar = 0x0;
        exc.dsisr = LR;
        nw4r::db::DumpException_(&exc);

    }
    
    kmWrite32(0x80026028, 0x60000000);
    kmWrite32(0x8002602C, 0x60000000);
    kmWrite32(0x80026038, 0x7c080378);
    kmCall(0x8002603c, HandlePanic);

    char filepath[20];

    char * GetRegionName()
    {
        switch (gameID[3])
        {
            default:
            case 'P':
                return "PAL";
                break;
            case 'E':
                return "NTSC-U";
                break;
            case 'J':
                return "NTSC-J";
                break;
        }

        return filepath;
    }

    void PrintPanic(u16 error, const OSContext * context, u32 dsisr, u32 dar)
    {
        char * region_name= GetRegionName();


        nw4r::db::Exception_Printf_("\n\n*** Mario Kart Wii Deluxe PANIC HANDLER ***\nOSPanic() has been called at 0x%08x (%s)\n<Symbol not found>\n\n", dsisr-4, region_name);
        nw4r::db::Exception_Printf_("*** Message ***\n%s\n", output);
    }

    void PrintContext(u16 error, const OSContext * context, u32 dsisr, u32 dar)
    {
        if(error != 0x30)
            nw4r::db::PrintContext_(error, context, dsisr, dar);
        else
            PrintPanic(error, context, dsisr, dar);
        return;
    }

    kmCall(0x80023484, PrintContext);

}