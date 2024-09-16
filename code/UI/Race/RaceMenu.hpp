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

#pragma once
#include "UI/Page/RaceMenu/VSPause.hpp"
#include <game/UI/Page/RaceMenu/TTPause.hpp>
#include "Race/RaceData.hpp"

namespace CosmosUI {

    enum CustomButtons {
        RaceMenu_ButtonSettings = 37,
    };

    static u32 TTPauseButtonArray[6] = {
        Pages::RaceMenu::ButtonContinue,
        Pages::RaceMenu::ButtonRestart1,
        Pages::RaceMenu::ButtonChangeCourse,
        Pages::RaceMenu::ButtonChangeCharacter,
        RaceMenu_ButtonSettings, 
        Pages::RaceMenu::ButtonQuit,
    };

    class TTPausePlus : public Pages::TTPause {
    public:
        TTPausePlus() {};

        u32 nextMenu;

        char* GetButtonsBRCTRName() const override { return "CosmosPauseMenuTimeAttack"; }
        u32 *GetVariantsIdxArray() const override { return TTPauseButtonArray; }
        u32 GetButtonCount() const override { return 6; }
    };

    static u32 VSPauseButtonArray[3] = {
        Pages::RaceMenu::ButtonContinue,
        RaceMenu_ButtonSettings,
        Pages::RaceMenu::ButtonQuit,
    };

    class VSPausePlus : public Pages::VSPause {
    public:
        VSPausePlus() {};

        u32 nextPage;

        u32 GetButtonCount() const override { return 3; };
        u32* GetVariantsIdxArray() const override { return VSPauseButtonArray; }
        char* GetButtonsBRCTRName() const override { 
            if(RaceData::GetStaticInstance()->racesScenario.localPlayerCount > 1) return "CosmosPauseMenuVSMulti";
            return "CosmosPauseMenuVS";
        }   
    };




}
