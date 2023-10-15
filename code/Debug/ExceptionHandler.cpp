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

    void SetConsoleParams(){
        nw4r::db::detail::ConsoleHead *console = EGG::Exception::console;
        console->viewLines = 0x16;
        console->viewPosY = 0x08;
    }
    BootHook ConsoleParams(SetConsoleParams, LOW);

}