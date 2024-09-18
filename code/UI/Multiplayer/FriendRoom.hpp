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

#include <game/UI/Page/Other/FriendRoom.hpp>

namespace Cosmos {
    namespace UI {
        class FriendRoomPlus : public Pages::FriendRoom {
        public:
            FriendRoomPlus() {
                this->onSettingsButtonClick.subject = this;
                this->onSettingsButtonClick.ptmf = &FriendRoomPlus::OnSettingsButton;
            };

            void OnInit() override;
            void OnResume() override;
            void AfterControlUpdate() override;

            void OnSettingsButton(PushButton* button, u32 hudSlotId);
            
            PushButton settingsButton;
            PtmfHolder_2A<FriendRoomPlus, void, PushButton*, u32> onSettingsButtonClick;
            bool controlsHidden;

        };
    }
}
