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

#ifndef _COSMOS_AUTHOR_CREDITS_
#define _COSMOS_AUTHOR_CREDITS_

#include <kamek.hpp>
#include <UI/RaceBase/ControlRaceBase.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceWifi.hpp>

#define BMGOFFSET 0x13000

namespace CosmosUI{
    class ControlAuthorCredits : public CtrlRaceWifiStartMessage
    {
    private:
        void Load();
    public:
        static u32 Count();
        static void Create(Page* page, u32 index);
    };
}

#endif
