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

#include "Race/RaceInfo.hpp"
#include <UI/RaceBase/AuthorCredits.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <core/rvl/os/OS.hpp>
#include <Aurora/AuroraSlot.hpp>

namespace CosmosUI{

    void ControlAuthorCredits::Create(Page * page, u32 index)
    {
        ControlAuthorCredits* credits = new(ControlAuthorCredits);
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

        if(trackId == Aurora::Special::SLOT_SANDY_CLOCKTOWER){
            Random random;
            if(random.NextLimited(50) == 0){
                this->SetMsgId(0x70001, 0);
            }
        }
    }
    bool ControlAuthorCredits::HasStarted(){
        s32 time = RaceInfo::GetStaticInstance()->timer;
        return time > 0 && time < 300;
    }
    bool ControlAuthorCredits::IsActive(){
        return !this->HasStarted();
    }

}
