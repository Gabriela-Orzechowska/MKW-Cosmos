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

#include <UI/RaceBase/AuthorCredits.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <core/rvl/os/OS.hpp>

namespace CosmosUI{

    u32 ControlAuthorCredits::Count()
    {
        return 1;
    }

    void ControlAuthorCredits::Create(Page * page, u32 index)
    {
        ControlAuthorCredits * credits = new(ControlAuthorCredits);
        page->AddControl(index, credits, 0);
        credits->Load();
    }

    static CustomControlBuilder AuthorPanel(ControlAuthorCredits::Count, ControlAuthorCredits::Create);

    void ControlAuthorCredits::Load()
    {
        this->hudSlotId = 0;
        ControlLoader loader(this);
        loader.Load("game_image", "CosmosCredits", "info_1", NULL);
        this->textBox_00 = this->layout.GetPaneByName("TextBox_00");
        u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
        this->SetMsgId(BMGOFFSET + trackId, 0);
    }

}
