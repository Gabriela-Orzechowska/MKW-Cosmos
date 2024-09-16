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

#ifndef _COSMOS_CTRLRACEBASE_
#define _COSMOS_CTRLRACEBASE_

#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceBase.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>

//Credits to Melg for SIP implementation

namespace CosmosUI{
    class CustomControlBuilder{
    private:
        typedef u32 (Func)();
        typedef void (Funcv)(Page * page, u32 index);

        Func * CountControls;
        Funcv * CreateControls;

        u32 controlCount;
        CustomControlBuilder * nextBuilder;
        static CustomControlBuilder * sHooks;

    public:
        static void BuildCustomRaceControl(Pages::RaceHUD * raceHudPage, u32 controlCount);
        CustomControlBuilder(Func * count, Funcv * create)
        {
            nextBuilder = sHooks;
            sHooks = this;
            CountControls = count;
            CreateControls = create;
        }
    private:
        static u32 Count()
        {
            u32 result = 0;
            for(CustomControlBuilder * builder = sHooks; builder; builder = builder->nextBuilder)
            {
                builder->controlCount = builder->CountControls();
                result += builder->controlCount;
            }
            return result;
        }
        static void CreateCustomControls(Page * page, u32 firstIdx)
        {
            u32 index = firstIdx;
            for(CustomControlBuilder * builder = sHooks; builder; builder = builder->nextBuilder)
            {
                if(builder->controlCount != 0)
                {
                    builder->CreateControls(page, index);
                    index += builder->controlCount;
                }
            }
        }
    };
}



#endif //_COSMOS_CTRLRACEBASE_
