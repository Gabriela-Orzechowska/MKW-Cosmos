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

//Credits to Melg for SIP implementation

#include <UI/RaceBase/ControlRaceBase.hpp>

namespace CosmosUI
{
    CustomControlBuilder * CustomControlBuilder::sHooks = NULL;

    void CustomControlBuilder::BuildCustomRaceControl(Pages::RaceHUD * raceHugPage, u32 controlCount)
    {
        u32 customControlCount = CustomControlBuilder::Count();
        raceHugPage->InitControlGroup(controlCount + customControlCount);
        CustomControlBuilder::CreateCustomControls(raceHugPage, controlCount);
        return;
    }

    kmCall(0x808562d0, CustomControlBuilder::BuildCustomRaceControl);

} // namespace CosmosUI
