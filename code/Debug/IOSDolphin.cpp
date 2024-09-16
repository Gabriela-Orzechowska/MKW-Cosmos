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

#include <Debug/IOSDolphin.hpp>

namespace IOS
{
    namespace Dolphin
    {
        static char result[0x10];
        char * GetVersion()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return nullptr;
            
            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = result;
            request.size = 0x0F;

            s32 ret = IOS::IOCtlv(devDolphin, (IOS::IOCtlType)GET_VERSION, 0, 1, &request);

            return result;
        }
        char * GetVersionName()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return nullptr;

            static char output[0x20];
            snprintf(output, 0x20, "Dolphin %s", GetVersion());
            return output;
        }

        u32 GetRunTime()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return 0;

            u32 runTime = 0;

            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = &runTime;
            request.size = sizeof(runTime);

            s32 ret = IOS::IOCtlv(devDolphin, (IOS::IOCtlType)GET_ELAPSED_TIME, 0, 1, &request);

            return runTime;
        }

        u32 GetCPUSpeed()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return 0;

            u32 cpuSpeed = 0;

            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = &cpuSpeed;
            request.size = sizeof(cpuSpeed);

            s32 ret = IOS::IOCtlv(devDolphin, (IOS::IOCtlType)GET_CPU_SPEED, 0, 1, &request);
            if(ret < 0) cpuSpeed = 0;

            return cpuSpeed;
        }

        u32 GetSpeedLimit()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return 0;

            u32 percent = 0;

            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = &percent;
            request.size = sizeof(percent);

            s32 ret = IOS::IOCtlv(devDolphin, (IOS::IOCtlType)GET_SPEED_LIMIT, 0, 1, &request);

            return percent;
        }
        s32 SetSpeedLimit(u32 percent)
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return 0;

            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = &percent;
            request.size = sizeof(percent);

            return IOS::IOCtlv(devDolphin, (IOS::IOCtlType)SET_SPEED_LIMIT, 1, 0, &request);
        }

        u64 GetSystemTime()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return 0;

            u64 time = 0;

            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = &time;
            request.size = sizeof(time);

            s32 ret = IOS::IOCtlv(devDolphin, (IOS::IOCtlType)GET_SYSTEM_TIME, 0, 1, &request);
            if(ret < 0) CosmosError("Failed to obtain SystemTime!\n");
            return time;
        }

        s32 DiscordSetClient(const char * client)
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return -1;

            IOCtlvRequest request __attribute__ ((aligned(0x20)));
            request.address = (void *) client;
            request.size = strlen(client) + 1;

            return IOS::IOCtlv(devDolphin, (IOS::IOCtlType)DISCORD_SET_CLIENT, 1, 0, &request);
        }

        s32 DiscordSetPresence(const DiscordRichPresence * presence)
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return 0;

            IOCtlvRequest request[10] __attribute__ ((aligned(0x20)));
            
            #define setString(x, y) request[x].address = (void *) y; request[x].size = strlen(y)+1;
            #define setVal(x, y) request[x].address = (void *) &y; request[x].size = sizeof(y);

            setString(0, presence->details);
            setString(1, presence->state);
            setString(2, presence->largeImageKey);
            setString(3, presence->largeImageText);
            setString(4, presence->smallImageKey);
            setString(5, presence->smallImageText);
            setVal(6, presence->startTimestamp);
            setVal(7, presence->endTimestamp);
            setVal(8, presence->partySize);
            setVal(9, presence->partyMax);

            return IOS::IOCtlv(devDolphin, (IOS::IOCtlType)DISCORD_SET_PRESENCE, 10, 0, request);
        }

        s32 DiscordReset()
        {
            if(devDolphin < 0) Open();
            if(devDolphin < 0) return 0;

            return IOS::IOCtlv(devDolphin, (IOS::IOCtlType)DISCORD_RESET, 0, 0, NULL);
        }
    }
}
