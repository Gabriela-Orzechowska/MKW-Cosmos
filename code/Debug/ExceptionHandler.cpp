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
#include <Debug/Debug.hpp>
#include <core/rvl/vi.hpp>
#include <core/egg/Thread.hpp>
#include <core/rvl/wpad.h>
#include <game/Input/InputData.hpp>
#include <game/Input/InputData.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Controller/MiscController.hpp>
#include <core/System/SystemManager.hpp>
#include <Debug/SymbolMap.hpp>
#include <main.hpp>
#include <System/Security.hpp>

extern char gameID[4];
extern nw4r::db::ExceptionInfo exceptionData; // 0x802A70B8

namespace CosmosDebug
{

    enum PanicType
    {
        NW4R_PANIC = 0x30,
        OS_PANIC,
        COSMOS_PANIC,
    };

    static char output[0x100];

    void HandlePanic(const char *file, int line, const char *fmt, va_list vlist, bool halt, u32 LR)
    {

        char format[0x100];
        snprintf(output, 0x100, "%s:%d Panic:\n", file, line);
        vsnprintf(format, 0x100, fmt, vlist);
        snprintf(output, 0x100, "%s%s", output, format);
        OSContext *context = OSGetCurrentContext();
        nw4r::db::ExceptionCallbackParam exc;
        exc.error = NW4R_PANIC;
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

    void HandleOSPanic(const char *file, u32 line, char *message)
    {
        snprintf(output, 0x100, "%s:%d Panic:\n%s", file, line, message);
        OSContext *context = OSGetCurrentContext();
        nw4r::db::ExceptionCallbackParam exc;
        exc.error = OS_PANIC;
        exc.context = context;
        exc.dar = (u32)output;
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

    char *GetRegionName()
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
    extern u32 SRR0_addr;

    void PrintPanic(u16 error, const OSContext *context, u32 dsisr, u32 dar)
    {
        char *region_name = GetRegionName();

        nw4r::db::Exception_Printf_("*** COSMOS PANIC HANDLER ***\n");
        if (error == NW4R_PANIC)
            nw4r::db::Exception_Printf_("nw4r::Panic()");
        else if (error == OS_PANIC)
            nw4r::db::Exception_Printf_("RVL::OSPanic()", region_name);
        else if (error == COSMOS_PANIC)
            nw4r::db::Exception_Printf_("Cosmos::Panic()", region_name);
        nw4r::db::Exception_Printf_(" has been called at 0x%08x (%s)\n<Symbol not found>\n\n", dsisr - 4, region_name);
        nw4r::db::Exception_Printf_("Platform: %s\nCosmos %s (%s %s)\n-------------------------------\n", CosmosDebug::GetPlatformName(), __COSMOS_VERSION__, __DATE__, __TIME__);


        nw4r::db::Exception_Printf_("*** Message ***\n%s\n", (char *)dar);
    }

    static void PrintStackTrace_(u32 spStart)
    {
        using namespace nw4r::db;
        Exception_Printf_("-------------------------------- TRACE\n");
        Exception_Printf_("Address:   Back Chain   LR Save\n");

        u32 *sp = (u32 *)spStart;
        u32 spAddr = spStart;
        for (int i = 0; i < 0x10; i++)
        {
            if (spAddr == 0 || spAddr == 0xFFFFFFFF)
                break;

            if ((spAddr & 0x30000000) == 0)
            {
                u32 mem1Size = OSGetPhysicalMem1Size();
                if ((spAddr & 0xfffffff) > mem1Size)
                    break;
            }
            else if ((spAddr & 0x30000000) == 1)
            {
                u32 mem2Size = OSGetPhysicalMem2Size();
                if ((spAddr & 0xfffffff) > mem2Size)
                    break;
            }
            else
                break;

            #ifdef COSMOS_SECURITY
            u32 sp1Val = Cosmos::Security::GetDemangledAddress(sp[1]);
            #else
            u32 sp1Val = sp[1];
            #endif

            Exception_Printf_("%08X:  %08X    %08X: %s\n", spAddr, sp[0], sp1Val, SymbolManager::GetSymbolName(sp1Val));
            sp = (u32 *)*sp;
            spAddr = (u32)sp;
        }
    }

    static void PrintContext_(u32 error, const OSContext *context, u32 dsisr, u32 dar)
    {
        IOS::Dolphin::SetSpeedLimit(100);

        using namespace nw4r::db;
        if (error >= 0x30)
        {
            PrintPanic(error, context, dsisr, dar);
            return;
        }
        OSReport("Size of ExceptionInfo: %d\nExceptionInfo addr: %p\n", sizeof(ExceptionInfo), &exceptionData);
        Exception_Printf_("**** COSMOS %s ****\n", error < 0x11 ? "EXCEPTION HANDLER" : "USER HALT");
        Exception_Printf_("Platform: %s (%s)\nCosmos %s (%s, %s %s)\n", CosmosDebug::GetPlatformName(), GetRegionName(), __COSMOS_VERSION__, __COMPILER_VERSION__, __DATE__, __TIME__);
        Exception_Printf_("Framebuffer: %08XH\n", (u32)exceptionData.framebuffer);
        Exception_Printf_("--------------------------------\n");

        if (exceptionData.displayInfo & EXCEPTION_INFO_MAIN)
        {
            Exception_Printf_("---EXCEPTION_INFO_MAIN---\n");
            Exception_Printf_("CONTEXT:%08XH  (%s EXCEPTION)\n", context, error < 0x11 ? ExceptionNameTable[error] : "UNKNOWN");
            Exception_Printf_("SRR0:   %08XH   SRR1:%08XH\n", context->srr0, context->srr1);
            if (context->srr0 == 0)
                Exception_Printf_("Missing SRR0! Submit Invalid Read/Write\n");
            else
                Exception_Printf_("%s\n", SymbolManager::GetSymbolName(context->srr0));
            Exception_Printf_("DSISR:  %08XH   DAR:  %08XH\n", dsisr, dar);
        }
        if (exceptionData.displayInfo & EXCEPTION_INFO_GPR)
        {
            Exception_Printf_("\n---EXCEPTION_INFO_GRP---\n");
            Exception_Printf_("--------------------------------\n");
            for (int i = 0; i < 10; i++)
            {
                Exception_Printf_("R%02d:%08XH R%02d:%08XH R%02d:%08XH\n", i, context->gpr[i], i + 0xb, context->gpr[i + 0xb], i + 0x16, context->gpr[i + 0x16]);
            }
            Exception_Printf_("R%02d:%08XH R%02d:%08XH\n", 10, context->gpr[10], 21, context->gpr[21]);
        }
        if (exceptionData.displayInfo & EXCEPTION_INFO_FPR)
        {
            Exception_Printf_("\n---EXCEPTION_INFO_FPR---\n");
            ShowFloat_(context);
        }
        if (exceptionData.displayInfo & EXCEPTION_INFO_TRACE)
        {
            Exception_Printf_("\n---EXCEPTION_INFO_TRACE---\n");
            PrintStackTrace_(exceptionData.sp);
        }
        Exception_Printf_("--------------------------------\n");
    }

    kmCall(0x80023484, PrintContext_);

    /*


            nw4r::db::Exception_Printf_("*** Dearest Player\n*** I hope it finds you well. We seem to have\n*** found ourselves a crash in the game. Please\n*** consider taking a screenshot and sending it \n*** to #bug-reports\n");
            nw4r::db::Exception_Printf_("***\n");
            nw4r::db::Exception_Printf_("*** Sincerely, @VolcanoPiece1\n");


    kmCall(0x800236c8, PrintHeader);

    */

#ifdef FORCEOSFATAL

    char output2[0x100];
    void PrintFATALContext(u16 error, const OSContext *context, u32 dsisr, u32 dar)
    {
        snprintf(output2, 0x100, "AN ERROR HAS OCCURED at %8x: ", context->srr0);

        u32 black = 0x000000FF;
        u32 white = 0xFFFFFFFF;

        OSFatal(&white, &black, output2);
    }

    kmCall(0x80023484, PrintFATALContext);

#else

#endif

    void SetExceptionParams()
    {
        using namespace nw4r::db;

        nw4r::db::detail::ConsoleHead *console = EGG::Exception::console;
        console->width = 0x70;
        console->height = 0x40;

        exceptionData.displayInfo |= EXCEPTION_INFO_GPR;
        exceptionData.displayInfo |= EXCEPTION_INFO_FPR;
    }
    BootHook ConsoleParams(SetExceptionParams, LOW);

    bool ExceptionCallBack_(nw4r::db::detail::ConsoleHead &head, void *)
    {
        s32 scrollCooldown = 200;

        OSReport("CALLBACK...\n");

        if (&head == nullptr)
        {
            CosmosError("No Console Found\n");
            return false;
        }
        VISetBlack(0);
        OSReport("cancel all thread...\n");
        EGG::Thread::kandoTestCancelAllThread();
        OSReport("done\n");

        OSDisableInterrupts();
        OSDisableScheduler();
        OSEnableInterrupts();

        s32 lineCount = head.ringTopLineCnt;
        s32 totalLineCount = head.GetLineCount();

        head.viewTopLine = lineCount;
        head.isVisible = true;
        head.DrawDirect();

        u32 controllerType;
        s32 ret = WPADProbe(0, &controllerType);

        bool ccp = (ret == 0 && (controllerType == 2 || controllerType == 7));

        u32 controller = MenuData::GetStaticInstance()->pad.padInfos[0].controllerSlotAndTypeActive;
        ControllerType type = ControllerType(controller & 0xFF);
        RealControllerHolder &holder = InputData::GetStaticInstance()->GetController(0);

        bool lock = true;

        while (true)
        {
            KPADStatus wStatus;
            PADStatus gcStatus[4];
            WPADCLStatus clStatus;

            KPADRead(0, &wStatus, 1);
            PADRead(&gcStatus);

            if (ccp)
            {
                KPADGetUnifiedWpadStatus(0, &clStatus, 1);
            }
            holder.inputStates[1] = holder.inputStates[0];
            switch (type)
            {
            case (CLASSIC):
                holder.inputStates[0].buttonRaw = clStatus.buttons;
                holder.inputStates[0].stickX = clStatus.lStickX / 128.0f;
                holder.inputStates[0].stickY = clStatus.lStickY / 128.0f;
                break;
            case (NUNCHUCK):
            case (WHEEL):
                holder.inputStates[0].buttonRaw = wStatus.buttons;
                holder.inputStates[0].stickX = wStatus.cStickHorizontal;
                holder.inputStates[0].stickY = wStatus.cStickVertical;
                break;
            case (GCN):
                holder.inputStates[0].buttonRaw = gcStatus[0].buttons;
                holder.inputStates[0].stickX = gcStatus[0].horizontalStickU8 / 62.0f;
                holder.inputStates[0].stickY = gcStatus[0].verticalStickU8 / 62.0f;
            default:
                break;
            }

            if (SystemManager::sInstance->doShutDown > 0)
            {
                Cosmos::System::Shutdown(true);
            }

            if (CosmosController::isPressed(holder, type, CosmosController::BUTTON_HOME) || CosmosController::isPressed(holder, type, CosmosController::BUTTON_PLUS))
            {
                Cosmos::System::Shutdown(true);
            }

            u32 tick0 = OSGetTick();
            u32 tick1 = tick0;
            while (OSTicksToMilliseconds(tick1 - tick0) < scrollCooldown)
                tick1 = OSGetTick();

            s32 xPos = head.viewPosX;
            s32 currentTopLine = head.viewTopLine;

            s32 prevXPos = xPos;
            s32 prevTopLine = currentTopLine;

            bool down = CosmosController::isPressed(holder, type, CosmosController::BUTTON_DPAD_DOWN);
            bool up = CosmosController::isPressed(holder, type, CosmosController::BUTTON_DPAD_UP);
            bool left = CosmosController::isPressed(holder, type, CosmosController::BUTTON_DPAD_LEFT);
            bool right = CosmosController::isPressed(holder, type, CosmosController::BUTTON_DPAD_RIGHT);

            if (lock)
            {
                currentTopLine++;
                if (currentTopLine == totalLineCount - (head.viewLines / 2))
                {
                    lock = false;
                    scrollCooldown = 100;
                    currentTopLine = 0;
                }
            }
            else
            {
                if (right)
                    xPos = Max(xPos - 5, -130);
                else if (left)
                    xPos = Min(xPos + 5, 10);

                if (down)
                    currentTopLine = Min(currentTopLine + 1, totalLineCount - head.viewLines);
                else if (up)
                    currentTopLine = Max(currentTopLine - 1, lineCount);
            }

            if (currentTopLine != prevTopLine || xPos != prevXPos)
            {
                head.viewPosX = xPos;
                head.viewTopLine = currentTopLine;
                head.DrawDirect();
            }
        }
    }
    kmBranch(0x80226464, ExceptionCallBack_);

}
