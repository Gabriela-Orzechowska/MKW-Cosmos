#ifndef _COSMOS_DEBUG_
#define _COSMOS_DEBUG_

#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>
#include <core/rvl/os/Error.hpp>
#include <core/rvl/os/OS.hpp>

namespace CosmosDebug
{
    enum HOSTPLATFORM
    {
        DOLPHIN,
        DOLPHIN_OLD,
        DOLPHIN_PREHISTORIC,
        DOLPHIN_UNKNOWN,
        WII,
        WII_U,
        WII_MINI,
    };


    const u32 DolphinECID_U = 0x0d96e200;

    static HOSTPLATFORM currentPlatform;
    static char platformName[0x20];

    static void DetectPlatform();
    char * GetPlatformString();
} // namespace Cosmos


#endif