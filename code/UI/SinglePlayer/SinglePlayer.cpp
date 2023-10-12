#include <UI/SinglePlayer/SinglePlayer.hpp>
#include <game/GlobalFunctions.hpp>
#include <game/UI/Page/Menu/SinglePlayer.hpp>
#include <game/UI/Text.hpp>
#include <game/Race/RaceData.hpp>
#include <main.hpp>

#pragma region SingleMenuPage

const char * ttButtonBrctr = "DX_TT";

void LoadCorrectControlFile(PushButton * button, const char * folderName, const char * controlName, const char * variantName, u32 param_5, u32 param_6, u32 idx)
{
    
    if(idx == TIME_TRIAL || idx == TIME_TRIAL_200)
    {
        char ttButtonVariant[17];
        snprintf(ttButtonVariant, 17, "%s_%d", ttButtonBrctr, idx);
        controlName = ttButtonBrctr;
        variantName = ttButtonVariant;
    }
    
    button->Load(folderName, controlName, variantName, param_5, param_6, false);
    ((Pages::SinglePlayer*)(button->parentGroup->parentPage))->curMovieCount = 0;
}

kmWrite16(0x806266c2, 0x5); 
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

void OnButtonSelect(Pages::SinglePlayer * page, PushButton * button, u32 slotId)
{
    if(button->buttonId == TIME_TRIAL_200) page->bottomText->SetMsgId(0x2820);
    else page->Pages::SinglePlayer::OnExternalButtonSelect(button, slotId);
}

kmWritePointer(0x808D9F64, OnButtonSelect);

void OnButtonClick(Pages::SinglePlayer * page, PushButton * button, u32 slotId)
{
    u32 buttonOriginalId = button->buttonId;
    if(buttonOriginalId == TIME_TRIAL_200) button->buttonId = TIME_TRIAL;
    page->Pages::SinglePlayer::OnButtonClick(button, slotId);
    button->buttonId = buttonOriginalId;
    if(buttonOriginalId == TIME_TRIAL_200) 
        DX::SetTTCC(DX::DX_TT_200CC);
 
    else if(buttonOriginalId == TIME_TRIAL)
        DX::SetTTCC(DX::DX_TT_150CC);
    
    return;
}
kmWritePointer(0x808BBED0, OnButtonClick);

#pragma endregion SingleMenuPage