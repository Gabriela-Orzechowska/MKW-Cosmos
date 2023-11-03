#ifndef _IOS_DOLPHIN_
#define _IOS_DOLPHIN_

#include <kamek.hpp>
#include <core/rvl/os/OS.hpp>
#include <main.hpp>

namespace IOS
{
    namespace Dolphin
    {
        enum DolRequest
        {
            GET_ELAPSED_TIME = 0x01,
            GET_VERSION = 0x02,
            GET_SPEED_LIMIT = 0x03,
            SET_SPEED_LIMIT = 0x04,
            GET_CPU_SPEED = 0x05,
            GET_REAL_PRODUCTCODE = 0x06,

            DISCORD_SET_CLIENT = 0x07,
            DISCORD_SET_PRESENCE = 0x08,

            GET_SYSTEM_TIME = 0x09,
        };

        static s32 devDolphin = -1;

        static s32 Open()
        {
            if(devDolphin < 0) devDolphin = DX::Open("/dev/dolphin", IOS::MODE_NONE);
            return devDolphin;
        }

        static bool IsOpen() { return devDolphin >= 0; }

        static bool Close()
        {
            if(devDolphin >= 0) IOS::Close(devDolphin);
            return true;
        }

        char * GetVersion();
        char * GetVersionName();
    }
}

#endif