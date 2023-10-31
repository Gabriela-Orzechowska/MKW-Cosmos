#ifndef _DX_DEBUG_
#define _DX_DEBUG_

#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>
#include <core/rvl/os/Error.hpp>
#include <core/rvl/os/OS.hpp>

namespace DXDebug
{
    enum HOSTPLATFORM
    {
        DOLPHIN,
        DOLPHIN_OLD,
        WII,
        WII_U,
        WII_MINI,
    };

    static HOSTPLATFORM currentPlatform;
    static char platformName[0x20];

    static void DetectPlatform();
    char * GetPlatformString();
} // namespace DX


#endif