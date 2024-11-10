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

#include "UI/Ctrl/MessageWindowControl.hpp"
#include <kamek.hpp>
#include <game/UI/Page/Other/Approve.hpp>
#include <game/UI/Page/Page.hpp>

namespace CosmosUI
{
    #define MAX_MESSAGE_COUNT 8

    enum MESSAGE_TYPE {
        RESTART_REQUIRED,
        INFO,
        SHUTDOWN,
    };

    class MessagePageWindow : public Pages::Approve
    {
    public:
        void OnInit() override;
        void OnBackPress(u32 hudSlotId);
        void OnClick(PushButton *button, u32 hudSlotId);
        void AddMessage(MESSAGE_TYPE type, u32 bmgid);

        
    private:
        void NextMessage();

        MESSAGE_TYPE type[MAX_MESSAGE_COUNT];
        u32 messageBmg[MAX_MESSAGE_COUNT];
        u32 messageCount;
    };

    class AwaitPageWithBackground : public Pages::AutoEnding {
    public:
        void OnInit() override;
        void SetMessageWindowText(u32 bmgId, const TextInfo *text) override {
            this->messageWindowScale.SetMsgId(bmgId, text);
        };

    protected:
        MessageWindowControlScaleFade messageWindowScale; 
        BlackBackControlForMessage blackBack;
    };
}
