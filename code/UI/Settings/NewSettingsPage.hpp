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
        NewSettings();
        // ~NewSettings();

        void OnInit() override;
        void OnActivate() override;

    private:
        void OnBack(u32 hudSlotId);
        void OnBackButtonClick(PushButton * button, u32 hudSlotId);

        void OnSettingsPageControlChange(TextUpDownValueControl::TextControl* valueControl, u32 hudSlotId);
        void OnSettingsPageControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnSettingsPageControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        void OnValueControlChange(TextUpDownValueControl::TextControl* valueControl, u32 hudSlotId);
        void OnValueControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnValueControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        void OnDummyDeselect(UpDownControl*,u32) {}

        ControlsManipulatorManager controlsManipulatorManager;
        CtrlMenuBackButton backButton;
        
        u8 currentPage;
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
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onDeselectHandler;
    };
} // namespace CosmosUI
