/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _IOS_DOLPHIN_
#define _IOS_DOLPHIN_

#include <main.hpp>
#include <kamek.hpp>
#include <core/rvl/os/OS.hpp>


// I need to make this a class

namespace IOS
{
    namespace Dolphin
    {
        typedef struct DiscordRichPresence {
            char* state; /* max 128 bytes */
            char* details; /* max 128 bytes */
            s64 startTimestamp;
            s64 endTimestamp;
            char* largeImageKey; /* max 32 bytes */
            char* largeImageText; /* max 128 bytes */
            char* smallImageKey; /* max 32 bytes */
            char* smallImageText; /* max 128 bytes */
            int partySize;
            int partyMax;
        } DiscordRichPresence;

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

            DISCORD_RESET = 0x09,

            GET_SYSTEM_TIME = 0x0A,
        };

        static s32 devDolphin = -1000;

        static inline s32 Open()
        {
            if(devDolphin == -1000) devDolphin = Cosmos::Open("/dev/dolphin", IOS::MODE_NONE);
            return devDolphin;
        }

        static inline bool IsOpen() { return Open() >= 0; }

        static inline bool Close()
        {
            if(devDolphin >= 0) IOS::Close(devDolphin);
            return true;
        }

        char * GetVersion();
        int GetNumericalVersionNumber();
        char * GetVersionName();
        u32 GetRunTime();
        u32 GetCPUSpeed();

        u32 GetSpeedLimit();
        s32 SetSpeedLimit(u32 percent);
        u64 GetSystemTime();

        s32 DiscordSetClient(const char * client);
        s32 DiscordSetPresence(const DiscordRichPresence * presence);
        s32 DiscordReset();
    }
}

#endif
