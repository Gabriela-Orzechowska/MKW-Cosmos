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

//#define FORCEOSFATAL

namespace DXDebug{

    static char output[0x100];

    void HandlePanic( const char *file, int line, const char *fmt, va_list vlist, bool halt, u32 LR)
    {
        char format[0x100];
        snprintf(output, 0x100, "%s:%d Panic:\n", file, line);
        vsnprintf(format, 0x100, fmt, vlist);
        snprintf(output, 0x100, "%s%s", output, format);
        OSContext * context = OSGetCurrentContext();
        nw4r::db::ExceptionCallbackParam exc;
        exc.error = 0x30; 
        exc.context = context;
        exc.dar = 0x0;
        exc.dsisr = LR;
        #ifndef FORCEOSFATAL
        nw4r::db::DumpException_(&exc);
        #else
        u32 black = 0x000000FF;
        u32 white = 0xFFFFFFFF;

        OSFatal(&white, &black, output);
        #endif
    }
    
    kmWrite32(0x80026028, 0x60000000);
    kmWrite32(0x8002602C, 0x60000000);
    kmWrite32(0x80026038, 0x7c080378);
    kmCall(0x8002603c, HandlePanic);

    void HandleOSPanic(char * file, u32 line, char * message)
    {
        snprintf(output, 0x100, "%s:%d Panic:\n%s", file, line, message);
        OSContext * context = OSGetCurrentContext();
        nw4r::db::ExceptionCallbackParam exc;
        exc.error = 0x31; 
        exc.context = context;
        exc.dar = 0x0;
        exc.dsisr = 0x0;
        #ifndef FORCEOSFATAL
        nw4r::db::DumpException_(&exc);
        #else
        u32 black = 0x000000FF;
        u32 white = 0xFFFFFFFF;

        OSFatal(&white, &black, output);
        #endif
    }

    kmBranch(0x801a2660, HandleOSPanic);

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

    void PrintHeader()
    {
        nw4r::db::Exception_Printf_("**** MKWDX EXCEPTION HANDLER ****\n");
        nw4r::db::Exception_Printf_("Mario Kart Wii Deluxe v9.0 (17/10/2023)\n");
        return;
    }

    kmCall(0x800236c8, PrintHeader);

    void PrintSRR(const char * format, u32 srr0, u32 srr1)
    {
       nw4r::db::Exception_Printf_("SRR0:   %08XH   SRR1:%08XH\n", srr0, srr1);
       nw4r::db::Exception_Printf_("<Symbol not found>\n");

    }

    kmCall(0x80023b8c, PrintSRR);

    void PrintPanic(u16 error, const OSContext * context, u32 dsisr, u32 dar)
    {
        char * region_name= GetRegionName();

        if(error == 0x30)
            nw4r::db::Exception_Printf_("\n\n*** Mario Kart Wii Deluxe PANIC HANDLER ***\nnw4r::Panic() has been called at 0x%08x (%s)\n<Symbol not found>\n\n", dsisr-4, region_name);
        else if(error == 0x31)
            nw4r::db::Exception_Printf_("\n\n*** Mario Kart Wii Deluxe PANIC HANDLER ***\nRVL::OSPanic() has been called; (%s)\n<Symbol not found>\n\n", region_name);
        nw4r::db::Exception_Printf_("*** Message ***\n%s\n", output);
    }


    #ifdef FORCEOSFATAL

    char output2[0x100];
    void PrintFATALContext(u16 error, const OSContext * context, u32 dsisr, u32 dar)
    {
        snprintf(output2, 0x100, "AN ERROR HAS OCCURED at %8x: ", context->srr0);

        u32 black = 0x000000FF;
        u32 white = 0xFFFFFFFF;

        OSFatal(&white, &black, output2);
    }
    
    kmCall(0x80023484, PrintFATALContext);

    #else
    

    void PrintContext(u16 error, const OSContext * context, u32 dsisr, u32 dar)
    {
        if(error != 0x30)
            nw4r::db::PrintContext_(error, context, dsisr, dar);
        else
            PrintPanic(error, context, dsisr, dar);
        return;
    }

    kmCall(0x80023484, PrintContext);

    #endif
}