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

    static HOSTPLATFORM currentPlatform = WII;
    static char platformName[0x20];

    static void DetectPlatform();
    HOSTPLATFORM GetPlatform();
    char* GetPlatformName();
} // namespace Cosmos


#endif
