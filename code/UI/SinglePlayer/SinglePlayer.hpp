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

#ifndef _COSMOS_UI_SINGLEPLAYER_
#define _COSMOS_UI_SINGLEPLAYER_

#include <kamek.hpp>
#include <game/GlobalFunctions.hpp>
#include <game/UI/Page/Menu/SinglePlayer.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/Text.hpp>
#include <game/Race/RaceData.hpp>
#include <main.hpp>

enum MAIN_SCREEN_IDX{
    GP = 0,
    TIME_TRIAL = 1,
    VS = 2,
    BATTLE = 3,
};

enum EXTRA_MAIN_SCREEN_IDX{
    TIME_TRIAL_200 = 4,
    SETTINGS = 5,
};

#endif //_COSMOS_UI_SINGLEPLAYER_
