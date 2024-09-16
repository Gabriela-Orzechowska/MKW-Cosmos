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
#include <game/Input/InputData.hpp>
#include <game/UI/MenuData/MenuData.hpp>

// This code is inspired by Bbg's VP implementation

namespace CosmosController
{
    enum ButtonCommon{
        BUTTON_NONE             = 0,
        BUTTON_DPAD_LEFT        = 1 << 0,
        BUTTON_DPAD_RIGHT       = 1 << 1,
        BUTTON_DPAD_DOWN        = 1 << 2,
        BUTTON_DPAD_UP          = 1 << 3,
        BUTTON_A                = 1 << 4,
        BUTTON_B                = 1 << 5,
        BUTTON_X                = 1 << 6,
        BUTTON_Y                = 1 << 7,
        BUTTON_MINUS            = 1 << 8,
        BUTTON_PLUS             = 1 << 9,
        BUTTON_HOME             = 1 << 10,
        BUTTON_1                = 1 << 11,
        BUTTON_2                = 1 << 12,
        BUTTON_L                = 1 << 13,
        BUTTON_R                = 1 << 14,
        BUTTON_C                = 1 << 15,
        BUTTON_Z                = 1 << 16,
        BUTTON_ZL               = 1 << 17,
        BUTTON_ZR               = 1 << 18,
    };

    bool isPressed(const RealControllerHolder& holder, ControllerType type, u32 button, bool checkNew);
    bool isPressed(const RealControllerHolder& holder, ControllerType type, u32 button);

    bool arePressed(const RealControllerHolder& holder, ControllerType type, ButtonCommon buttons, bool checkNew);
    bool arePressed(const RealControllerHolder& holder, ControllerType type, ButtonCommon buttons);
}
