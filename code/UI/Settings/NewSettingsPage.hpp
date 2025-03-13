/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2025 Gabriela Orzechowska
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

/*
 * This settings menu style is inspired by MKW-SP implementation, while 
 * all the assets are custom made. I would like to thank stebler for
 * direct help gettings this menu to work.
 * 
 * https://github.com/mkw-sp/mkw-sp/blob/main/payload/game/ui/SettingsPage.cc
 * 
 * The original license text goes as follows:
 * 
 * Copyright 2021-2023 Pablo Stebler
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */



#pragma once
#include <UI/MenuDataPlus.hpp>
#include "UI/Ctrl/CountDown.hpp"
#include "System/System.hpp"
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Ctrl/UpDown.hpp>
#include <Settings/UserData.hpp>
#include <UI/BMG/BMG.hpp>


namespace CosmosUI
{
    class NewSettings : public Page {
    public:
        NewSettings();
        ~NewSettings() override {}

        void OnInit() override;
        void OnActivate() override;
        void BeforeEntranceAnimations() override;
        void OnUpdate() override;
        PageId GetNextPage() const override { return this->returnPage; }

        static inline NewSettings* GetPage() { return MenuData::GetStaticInstance()->GetAuroraPage<NewSettings>(Aurora::SETTINGS_MAIN); }
        static void SetPreviousPageGlobal(PageId id, MenuId menu){
            NewSettings* page = NewSettings::GetPage();
            if(page != nullptr){
                page->SetPreviousPage(id, menu);
            }
        }
        void SetPreviousPage(PageId id, MenuId menu, PageId timer = PAGE_NONE) { this->returnPage = id; this->returnMenu = menu; this->timerPage = timer; }
        void SetTimer(CountDown* countDown) { this->timer = countDown; } 
        void SetNextSettingPage(u32 page) { 
            this->currentPage = page;
            this->pageSelector.curSelectedOption = page;
        }
        MenuId GetPreviousMenu() const { return this->returnMenu; }

    private:
        void OnBack(u32 hudSlotId);
        void OnBackButtonClick(PushButton * button, u32 hudSlotId);

        void OnSettingsPageControlChange(TextUpDownValueControl::TextControl* valueControl, u32 optionId);
        void OnSettingsPageControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnSettingsPageControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        void OnValueControlChange(UpDownControl* upDownControl, u32 hudSlotId, u32 index);
        void OnValueControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnValueControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        void OnDummyDeselect(UpDownControl*,u32) {}

        void ChangePage(s8 page = -1);

        ControlsManipulatorManager controlsManipulatorManager; //0x48
        CtrlMenuBackButton backButton; //0x268
    
        UpDownControl pageSelector; //0x4CC
        TextUpDownValueControl textPageSelector; //0xA94
        UpDownControl settingSelectors[6]; // 0xF08
        TextUpDownValueControl textSettingSelector[6]; //0x31B8

        CtrlMenuPageTitleText title;
        CtrlMenuInstructionText bottomText;

        PtmfHolder_1A<Page, void, u32> onBackPressHandler;       
        PtmfHolder_2A<Page, void, PushButton*, u32> onBackButtonPress; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onPageClickHandler; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onPageSelectHandler; 
        PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl*, u32> onPageChangeHandler; 

        PtmfHolder_2A<Page, void, UpDownControl*, u32> onValueSettingClickHandler;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onValueSettingSelectHandler;
        PtmfHolder_3A<Page, void, UpDownControl*, u32, u32> onValueSettingChangeHandler;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onDeselectHandler;
        CountDown* timer;

        u8 currentPage;
        PageId returnPage;
        MenuId returnMenu;
        PageId timerPage;
    };
} // namespace CosmosUI
