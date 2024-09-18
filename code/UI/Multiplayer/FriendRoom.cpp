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

#include "UI/Page/Other/FriendRoom.hpp"
#include "System/identifiers.hpp"
#include "main.hpp"
#include <UI/Multiplayer/FriendRoom.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <game/Visual/GlobeMgr.hpp>

namespace Cosmos {
    namespace UI {

        FriendRoomPlus* CreatePage(){
            return new FriendRoomPlus;
        }
        kmCall(0x8062420c, CreatePage);
        kmWrite32(0x80624200, 0x60000000);

        void FriendRoomPlus::OnInit() {
            Pages::FriendRoom::OnInit();
            this->AddControl(5, &this->settingsButton, false);
            this->settingsButton.Load("button", "CosmosFriendRoom", "Settings", 1, 0, false);
            this->settingsButton.SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton*, u32>*)&this->onSettingsButtonClick, false);
            this->settingsButton.buttonId = 5;
        }
        kmWrite16(0x805d825c + 2, 0x6); //Add one more child
                                        
        void FriendRoomPlus::OnResume(){
            if(this->controlsHidden) GlobeMgr::GetStaticInstance()->DisplayMii();
            this->controlsHidden = false;
            Pages::FriendRoom::OnResume();
        }

        void FriendRoomPlus::OnSettingsButton(PushButton* button, u32 hudslotid){
            CosmosUI::NewSettings::SetPreviousPageGlobal(PAGE_NONE, MENU_NONE);
            this->controlsHidden = true;
            this->AddPageLayer((PageId)Cosmos::SETTINGS_MAIN, 0);
        }

        void FriendRoomPlus::AfterControlUpdate(){
            Pages::FriendRoom::AfterControlUpdate();
            bool hid = this->controlsHidden;

            Pages::FriendRoomManager* manager = Pages::FriendRoomManager::GetPage();
            Pages::FriendRoomWaiting* waiting = Pages::FriendRoomWaiting::GetPage();

            messagesButton.isHidden = hid;
            settingsButton.isHidden = hid;

            backButton.isHidden = hid;
            bottomText.isHidden = hid;

            manager->titleText.isHidden = hid;
            for(int i = 0; i < 24; i++) manager->miiIcons[i].isHidden = hid;

            if(hid){
                startButton.isHidden = hid;
                addFriendsButton.isHidden = hid;
                waiting->messageWindow.isHidden = hid;
            }
        }

        void PatchButtonLoading(PushButton& button, const char* folder, const char*,
                const char* variant){
            button.Load(folder, "CosmosFriendRoom", variant, 1, 0, false);
        }
        kmCall(0x805d8298, PatchButtonLoading);
        kmCall(0x805d82f4, PatchButtonLoading);
        kmCall(0x805d834c, PatchButtonLoading);

    }
}
