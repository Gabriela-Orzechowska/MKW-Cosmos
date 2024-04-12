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
        void OnBack();

        CtrlMenuBackButton backButton;
        ControlsManipulatorManager controlsManipulatorManager;

        UpDownControl pageSelector;
        TextUpDownValueControlPlus textPageSelector;
        UpDownControl settingSelectors[6];
        TextUpDownValueControlPlus textSettingSelector[6];

        CtrlMenuPageTitleText title;
        CtrlMenuInstructionText bottomText;

        PtmfHolder_1A<NewSettings, void, u32> onBackPressHandler;

    };
} // namespace CosmosUI
