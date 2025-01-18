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
        u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
        
        ControlLoader loader(this);
        if(trackId != Aurora::Special::SLOT_TGAW) {
            loader.Load("game_image", "CosmosCredits", "info_1", NULL);
            this->textBox_00 = this->layout.GetPaneByName("TextBox_00");
            this->SetMsgId(BMGOFFSET + trackId, 0);

            if(trackId == Aurora::Special::SLOT_SANDY_CLOCKTOWER){
                Random random;
                if(random.NextLimited(50) == 0){
                    this->SetMsgId(0x70001, 0);
                }
            }
            if(trackId == Aurora::Special::SLOT_BBB){
                Random random;
                if(random.NextLimited(50) == 0){
                    this->SetMsgId(0x70007, 0);
                }
            }
        }
        else {
            loader.Load("game_image", "CosmosCreditsAW", "info_1", NULL);
            this->textBox_00 = this->layout.GetPaneByName("TextBox_00");
            this->SetTextBoxMsg("TextBox_00", 0x70003, nullptr);
            this->SetTextBoxMsg("TextBox_01", 0x70004, nullptr);
            this->SetTextBoxMsg("TextBox_02", 0x70005, nullptr);
            this->SetTextBoxMsg("TextBox_03", 0x70006, nullptr);
            CosmosLog("Pane pointer: %p\n", this->layout.GetPaneByName("TextBox_01"));
        }
    }

    void ControlAuthorCredits::OnUpdate(){
        CtrlRaceWifiStartMessage::OnUpdate();
        u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
        if(trackId != Aurora::Special::SLOT_TGAW) return;

        s32 time = RaceInfo::GetStaticInstance()->timer;
        if(time >= 200 && time < 2040) {
            this->layout.GetPaneByName("TextBox_01")->trans.x -= 1.0f;
            this->layout.GetPaneByName("TextBox_02")->trans.x -= 1.0f;
            this->layout.GetPaneByName("TextBox_03")->trans.x -= 1.0f;
        }
    };

    bool ControlAuthorCredits::HasStarted(){
        s32 time = RaceInfo::GetStaticInstance()->timer;
// AURORA
        u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
        if(trackId == Aurora::Special::SLOT_TGAW) {
            bool isEnd = (time < 2040) || (this->layout.GetPaneByName("TextBox_01")->trans.x < -1800.0f);
            return (time > 0) && isEnd;
        }
        return time > 0 && time < 300;
    }
    bool ControlAuthorCredits::IsActive(){
        return !this->HasStarted();
    }

}
