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

#include <UI/SinglePlayer/SinglePlayer.hpp>
#include <UI/Settings/NewSettingsPage.hpp>

#pragma region SingleMenuPage

const char * ttButtonBrctr = "CosmosTT";
const char * normalBrctr = "CosmosSingleTop";

void LoadCorrectControlFile(PushButton& button, const char * folderName, const char * controlName, const char * variantName, u32 param_5, u32 param_6, u32 idx)
{
    controlName = normalBrctr;
    if(idx == TIME_TRIAL || idx == TIME_TRIAL_200)
    {
        char ttButtonVariant[17];
        snprintf(ttButtonVariant, 17, "%s_%d", ttButtonBrctr, idx);
        controlName = ttButtonBrctr;
        variantName = ttButtonVariant;
    }
    else if(idx == SETTINGS)
    {
        variantName = "ButtonSettings";
    }
    
    button.Load(folderName, controlName, variantName, param_5, param_6, false);
    ((Pages::SinglePlayer*)(button.parentGroup->parentPage))->curMovieCount = 0;
}

kmWrite16(0x806266c2, 0x6); 
kmWrite32(0x8084f080,0x7F89E378); // mr r9, r28
kmCall(0x8084f084, LoadCorrectControlFile);

int FixDistanceFunc(ControlManipulator *main, ControlManipulator *dest, Directions direction, 
                    bool horizontalWrap, bool verticalWrap){
    u32 buttonId = ((CtrlMenuSingleTopMovieButton*)(main->actionHandlers[0]->subject))->buttonId;
    u32 destId = ((CtrlMenuSingleTopMovieButton*)(dest->actionHandlers[0]->subject))->buttonId;
    switch(buttonId){
        case GP:
            if(direction == DIRECTION_DOWN && destId == 1) return 1;
            break;
        case VS:
            if(direction == DIRECTION_UP && destId == 1) return 1;
            break;
        case TIME_TRIAL:
        case TIME_TRIAL_200:
            if(direction == DIRECTION_UP && destId == 0 || direction == DIRECTION_DOWN && destId == 2) return 1;
    }
    return main->CalcDistanceBothWrapping(dest, direction);
}

void FixMenuNagivation(ControlsManipulatorManager *mgr){
    mgr->calcDistanceFunc = &FixDistanceFunc;
}
kmCall(0x8084ef68, FixMenuNagivation);

void OnButtonSelect(Pages::SinglePlayer& page, PushButton * button, u32 slotId)
{
    if(button->buttonId == TIME_TRIAL_200) page.bottomText->SetMsgId(0x2820);
    else if(button->buttonId == TIME_TRIAL_200) page.bottomText->SetMsgId(0x2a20);
    else page.Pages::SinglePlayer::OnExternalButtonSelect(button, slotId);
}

kmWritePointer(0x808D9F64, OnButtonSelect);

void OnButtonClick(Pages::SinglePlayer& page, PushButton * button, u32 slotId)
{
    u32 buttonOriginalId = button->buttonId;
    if(buttonOriginalId == TIME_TRIAL_200) button->buttonId = TIME_TRIAL;

    if(button->buttonId == SETTINGS)
    {
        CosmosUI::NewSettings::SetPreviousPageGlobal(SINGLE_PLAYER_MENU, SINGLE_PLAYER_FROM_MENU);
        page.nextPageId = (PageId) Cosmos::SETTINGS_MAIN;
        page.EndStateAnimate(button->GetAnimationFrameSize(), 0);
    }
    else
    {
        page.Pages::SinglePlayer::OnButtonClick(button, slotId);
        button->buttonId = buttonOriginalId;
        if(buttonOriginalId == TIME_TRIAL_200) 
            Cosmos::System::GetStaticInstance()->SetTTMode(Cosmos::COSMOS_TT_200cc);
    
        else if(buttonOriginalId == TIME_TRIAL)
            Cosmos::System::GetStaticInstance()->SetTTMode(Cosmos::COSMOS_TT_150cc);
    }

    return;
}
kmWritePointer(0x808BBED0, OnButtonClick);

#pragma endregion SingleMenuPage
