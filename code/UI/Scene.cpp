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

#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>
#include <core/egg/Fader.hpp>

namespace Cosmos
{
    static bool isSpedUp = false;
    u32 SetDolphinSpeed(EGG::ColorFader* fader, u32, u32, u32 ret){
        if(IOS::Dolphin::IsOpen())
        {
            if(fader->status == EGG::Fader::CAN_FADE_IN) { 
                IOS::Dolphin::SetSpeedLimit(0);
            }

            else {
                IOS::Dolphin::SetSpeedLimit(100);
            }
        }

        return ret;
    }
    kmBranch(0x802153b0, SetDolphinSpeed);
} // namespace Cosmos
