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

#include "core/rvl/ipc/ipc.hpp"
#include "kamek.hpp"
#include "main.hpp"
#include <Debug/Debug.hpp>

#define vWIIMENU 0x0000000100000200


namespace CosmosDebug
{
    void DetectPlatform()
    {
        if(IOS::Dolphin::Open() >= 0)
        {
            currentPlatform = DOLPHIN;
            if(!IOS::Dolphin::GetVersion()) currentPlatform = DOLPHIN_OLD;
            IOS::Dolphin::Close();
            return;
        }

        IOS::IPCResult ret = IOS::Open("/dev/sha", IOS::MODE_NONE);
        if(ret == IOS::IPC_ENOENT){
            currentPlatform = DOLPHIN_UNKNOWN;
            return;
        }
        IOS::Close(ret);

        ret = Cosmos::Open("/title/00000001/00000002/data/macaddr.bin", IOS::MODE_READ);
        if(ret == IOS::IPC_OK)
        {
            IOS::Close(ret);
            currentPlatform = WII_MINI;
            return;
        }
        if(OSIsTitleInstalled(vWIIMENU))
        {
            currentPlatform = WII_U;
        }
        else
        {
            currentPlatform = WII;
        }
        return;
    }

    char* GetPlatformName() { return platformName; }

    HOSTPLATFORM GetPlatform() { return currentPlatform; }

    void UpdatePlatformString()
    {
        DetectPlatform(); 
        char* platformString;
        switch(currentPlatform)
        {
            case DOLPHIN:
                platformString = IOS::Dolphin::GetVersionName();
                break;
            case DOLPHIN_OLD:
                platformString = "Dolphin (OUTDATED)";
                break;
            case WII_MINI:
                platformString = "Wii Mini";
                break;
            case WII_U:
                platformString = "Wii U";
                break;
            case WII:
                platformString = "Wii";
                break;
        }
        CosmosLog("Platform: %s\n", platformString);
        strncpy(platformName, platformString, 0x20);
    }

    static BootHook SetupDebug(UpdatePlatformString, MEDIUM);
}
