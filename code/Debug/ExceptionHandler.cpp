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

namespace DXDebug{

    //Credit Star and Riidefi
    kmWrite16(0x802A7410, 0x00000023);
    kmWrite32(0x802A7404, 0x00000000);
    kmWrite32(0x80023948, 0x281e0007);
    kmWrite32(0x80009324, 0x38800068);

    void SetConsoleParams(){
        nw4r::db::detail::ConsoleHead *console = EGG::Exception::console;
        console->viewLines = 0x16;
        console->viewPosY = 0x08;
        console->viewPosX = 0x08;
    }
    BootHook ConsoleParams(SetConsoleParams, LOW);

}