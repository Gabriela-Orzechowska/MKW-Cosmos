#pragma once
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Ctrl/UpDown.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>


namespace CosmosUI
{
    class NewSettings : public Page {
    public:
        NewSettings() {
            SetupHandler(onBackPressHandler, void (Page::*)(u32), &NewSettings::OnBack);
            SetupHandler(onBackButtonPress, void (Page::*)(PushButton*,u32), &NewSettings::OnBackButtonClick);

            SetupHandler(onPageChangeHandler, void (Page::*)(TextUpDownValueControl::TextControl*, u32), &NewSettings::OnSettingsPageControlChange);
            SetupHandler(onPageClickHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnSettingsPageControlClick);
            SetupHandler(onPageSelectHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnSettingsPageControlSelect);
            
            SetupHandler(onValueSettingChangeHandler, void (Page::*)(TextUpDownValueControl::TextControl*,u32), &NewSettings::OnValueControlChange);
            SetupHandler(onValueSettingClickHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnValueControlClick);
            SetupHandler(onValueSettingSelectHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnValueControlSelect);
        }
        void OnInit() override;
        void OnActivate() override;

    private:
        u8 currentPage;
        void OnBack(u32 hudSlotId);
        void OnBackButtonClick(PushButton * button, u32 hudSlotId);

        void OnSettingsPageControlChange(TextUpDownValueControl::TextControl* valueControl, u32 hudSlotId);
        void OnSettingsPageControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnSettingsPageControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        void OnValueControlChange(TextUpDownValueControl::TextControl* valueControl, u32 hudSlotId);
        void OnValueControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnValueControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        CtrlMenuBackButton backButton;
        ControlsManipulatorManager controlsManipulatorManager;

        UpDownControl pageSelector;
        TextUpDownValueControl textPageSelector;
        UpDownControl settingSelectors[6];
        TextUpDownValueControl textSettingSelector[6];

        CtrlMenuPageTitleText title;
        CtrlMenuInstructionText bottomText;

        PtmfHolder_1A<Page, void, u32> onBackPressHandler;       
        PtmfHolder_2A<Page, void, PushButton*, u32> onBackButtonPress; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onPageClickHandler; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onPageSelectHandler; 
        PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl*, u32> onPageChangeHandler; 

        PtmfHolder_2A<Page, void, UpDownControl*, u32> onValueSettingClickHandler;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onValueSettingSelectHandler;
        PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl*, u32> onValueSettingChangeHandler;
    };
} // namespace CosmosUI
