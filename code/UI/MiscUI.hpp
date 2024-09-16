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

#ifndef _MISCUI_
#define _MISCUI_
#include <kamek.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>
#include <game/Sound/RSARSounds.hpp>


static bool loadedAsKorean = false;

namespace CosmosUI
{
    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const char * tpl);
    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const void * tplPointer);
}

static bool isOnlinePaused = false;

#endif
