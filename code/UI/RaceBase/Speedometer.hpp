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

#ifndef _COSMOS_SPEEDOMETER_
#define _COSMOS_SPEEDOMETER_

#include <kamek.hpp>
#include <game/Race/Kart/KartHolder.hpp>
#include <game/UI/MenuData/Menudata.hpp>
#include <UI/RaceBase/ControlRaceBase.hpp>
#include <Settings/UserData.hpp>

namespace CosmosUI
{    
    class ControlRaceSpeedometer : public CtrlRaceBase
    {
    private:

        void Load(const char* variant, u8 id);

    public:

        void Init() override;
        void OnUpdate() override;
        void SetSpeedoVariant(u8 variant);
        static u32 Count();
        static void Create(Page * page, u32 index);
        
    private:
        s8 places[5];
    };

    static ControlRaceSpeedometer* speedos[4] = {nullptr, nullptr, nullptr, nullptr};
}

#endif
