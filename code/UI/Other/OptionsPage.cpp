#include <UI/Other/OptionsPage.hpp>
#include <UI/Settings/SettingsBasePage.hpp>
#include <main.hpp>

namespace DXUI
{
    OptionsPlus * CreateOptions()
    {
        return new(OptionsPlus);
    }
    kmCall(0x8062453c, CreateOptions);
    kmWrite32(0x80624530,0x60000000); 
    kmWrite32(0x805fd754,0x60000000);

    void OptionsPlus::OnInit()
    {
        this->InitControlGroup(5 + 1);
        Options::OnInit();

        this->AddControl(this->controlGroup.controlCount - 1, &settingsButton, 0);
        this->settingsButton.Load("button", "DXOptions", "Settings", 1, 0, false);
        this->settingsButton.buttonId = 5;
        this->settingsButton.SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*)&this->onButtonClickHandler, 0);
        this->settingsButton.SelectInitialButton(0);
    }

    void OptionsPlus::OnPlusButtonClick(PushButton * button, u32 slotId)
    {
        if(button->buttonId == 5)
        {
            MenuData::sInstance->curScene->Get<SettingsBasePage>((PageId)DX::SETTINGS_MAIN)->lastPage = OPTIONS;
            MenuData::sInstance->curScene->Get<SettingsBasePage>((PageId)DX::SETTINGS_MAIN)->lastMenu = OPTIONS_MENU;
            this->nextPageId = (PageId) DX::SETTINGS_MAIN;
            this->EndStateAnimate(button->GetAnimationFrameSize(), 0);
        }
        else
        {
            this->OnButtonClick(button, slotId);
        }
    }
    void PatchBRCTR(PushButton *button, const char*folderName, const char*ctrName, const char*variant, u32 playerCount, u32 param_6, bool inaccessible){
        button->Load(folderName, "DXOptions", variant, playerCount, param_6, inaccessible);
    }
    kmCall(0x805fd7bc, PatchBRCTR);
    kmCall(0x805fd80c, PatchBRCTR);
    kmCall(0x805fd858, PatchBRCTR);
}