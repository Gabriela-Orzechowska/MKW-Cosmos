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
#include <game/Race/Kart/KartStatus.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <main.hpp>
#include <game/Race/RaceData.hpp>

void FastFallCalc(KartStatus& status, KartPhysics& physics)
{
    if(RaceData::GetStaticInstance()->racesScenario.GetSettings().engineClass == CC_100) {

        if(status.airtime >= 2)
        {
            if(!status.bool_0x96 || (status.airtime >= 20))
            {
                ControllerHolder * holder = status.base->GetControllerHolder();
                float y_input = holder->inputStates[0].stickY;
                if(y_input <= 0.0f) y_input = 0.0f;
                else y_input *= 2;
                physics.gravity -= y_input * 0.39f;
            }
        }

    }
    status.UpdateFromInput();
}

kmCall(0x805967a4, FastFallCalc);
