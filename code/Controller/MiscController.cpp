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

#include <Controller/MiscController.hpp>

// This code is inspired by Bbg's VP implementation

namespace CosmosController
{
    
    #define CASE_BUTTON(universal, specific) \
    case universal: return (raw & specific) != 0;
        
    bool isPressed(const RealControllerHolder& holder, ControllerType type, u32 button)
    {
        return isPressed(holder,type,button,false);
    }

    bool isPressed(const RealControllerHolder& holder, ControllerType type, u32 button, bool checkNew)
    {
        u16 raw = holder.inputStates[0].buttonRaw;
        if(checkNew) raw &= ~holder.inputStates[1].buttonRaw;

        if(type == WHEEL)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, WHEEL_A)
                CASE_BUTTON(BUTTON_B, WHEEL_B)
                CASE_BUTTON(BUTTON_1, WHEEL_1)
                CASE_BUTTON(BUTTON_2, WHEEL_1)
                CASE_BUTTON(BUTTON_MINUS, WHEEL_MINUS)
                CASE_BUTTON(BUTTON_PLUS, WHEEL_PLUS)
                CASE_BUTTON(BUTTON_HOME, WHEEL_HOME)
                CASE_BUTTON(BUTTON_DPAD_UP, WHEEL_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, WHEEL_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, WHEEL_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT, WHEEL_DPAD_RIGHT)
                default:
                    return false;
            }
        }
         
        else if(type == NUNCHUCK)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, NUNCHUCK_A)
                CASE_BUTTON(BUTTON_B, NUNCHUCK_B)
                CASE_BUTTON(BUTTON_C, NUNCHUCK_C)
                CASE_BUTTON(BUTTON_Z, NUNCHUCK_Z)
                CASE_BUTTON(BUTTON_1, NUNCHUCK_1)
                CASE_BUTTON(BUTTON_2, NUNCHUCK_2)
                CASE_BUTTON(BUTTON_MINUS, NUNCHUCK_MINUS)
                CASE_BUTTON(BUTTON_PLUS, NUNCHUCK_PLUS)
                CASE_BUTTON(BUTTON_DPAD_UP, NUNCHUCK_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, NUNCHUCK_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, NUNCHUCK_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT,  NUNCHUCK_DPAD_RIGHT)
                default:
                    return false;
            }
        }
        else if(type == CLASSIC)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, CLASSIC_A)
                CASE_BUTTON(BUTTON_B, CLASSIC_B)
                CASE_BUTTON(BUTTON_X, CLASSIC_X)
                CASE_BUTTON(BUTTON_Y, CLASSIC_Y)
                CASE_BUTTON(BUTTON_R, CLASSIC_R)
                CASE_BUTTON(BUTTON_L, CLASSIC_L)
                CASE_BUTTON(BUTTON_ZL, CLASSIC_ZL)
                CASE_BUTTON(BUTTON_ZR, CLASSIC_ZR)
                CASE_BUTTON(BUTTON_MINUS, CLASSIC_MINUS)
                CASE_BUTTON(BUTTON_HOME, CLASSIC_HOME)
                CASE_BUTTON(BUTTON_PLUS, CLASSIC_PLUS)
                CASE_BUTTON(BUTTON_DPAD_UP, CLASSIC_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, CLASSIC_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, CLASSIC_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT, CLASSIC_DPAD_RIGHT)
                default:
                    return false;
            }
        }
        else if(type==GCN)
        {
            switch(button) {
                CASE_BUTTON(BUTTON_A, GCN_A)
                CASE_BUTTON(BUTTON_B, GCN_B)
                CASE_BUTTON(BUTTON_X, GCN_X)
                CASE_BUTTON(BUTTON_Y, GCN_Y)
                CASE_BUTTON(BUTTON_L, GCN_L)
                CASE_BUTTON(BUTTON_R, GCN_R)
                CASE_BUTTON(BUTTON_Z, GCN_Z)
                CASE_BUTTON(BUTTON_PLUS, GCN_START)
                CASE_BUTTON(BUTTON_DPAD_UP, GCN_DPAD_UP)
                CASE_BUTTON(BUTTON_DPAD_DOWN, GCN_DPAD_DOWN)
                CASE_BUTTON(BUTTON_DPAD_LEFT, GCN_DPAD_LEFT)
                CASE_BUTTON(BUTTON_DPAD_RIGHT, GCN_DPAD_RIGHT)
                default:
                    return false;
            }
        }
                
        return false;
    }

    bool arePressed(const RealControllerHolder& holder, ControllerType type, ButtonCommon buttons)
    {
        return arePressed(holder, type, buttons, false);
    }

    bool arePressed(const RealControllerHolder& holder, ControllerType type, ButtonCommon buttons, bool checkNew)
    {
        bool result = true;
        for(int i = 0; i < 32; i++)
        {
            int mask = 1 << i;
            if(buttons & mask) {
                if(!isPressed(holder, type, (ButtonCommon)mask, checkNew))
                    result = false;
            }
        }
        return result;
    }
}

kmWrite32(0x80520318,0x71240808);
kmWrite32(0x8051f670,0x70e40081);
