#pragma once
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Ctrl/UpDown.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>


namespace CosmosUI
{
    class TextUpDownValueControlPlus : public TextUpDownValueControl
    {
        public:
        TextUpDownValueControlPlus(){};
        void SetMessage(u32 id) override;
        u32 id;
    };

    class NewSettings : public Page {
    public:
        NewSettings();
        ~NewSettings();

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
        TextUpDownValueControlPlus textPageSelector;
        UpDownControl settingSelectors[6];
        TextUpDownValueControlPlus textSettingSelector[6];

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
