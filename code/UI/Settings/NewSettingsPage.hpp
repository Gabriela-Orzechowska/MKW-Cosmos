#pragma once
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Ctrl/UpDown.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>


namespace CosmosUI
{
    typedef struct SettingPageOption{
        u8 optionCount;
        bool isBool;
    } SettingPageOption;

    typedef struct SettingPageDefinition{
        u8 settingCount;
        SettingPageOption settings[8];
    } SettingPageDefinition;

    class TextUpDownValueControlPlus : public TextUpDownValueControl
    {
        public:
        TextUpDownValueControlPlus(){};
        void SetMessage(u32 id) override;
        u32 id;
    };

    static SettingPageDefinition Definitions[3] = {
        {
            .settingCount = 3,
            .settings = { { .optionCount = 3, .isBool = false }, 
            { .optionCount = 2, .isBool = true },
            { .optionCount = 3, .isBool = false}  }
        }, {
            .settingCount = 2,
            .settings = { { .optionCount = 13, .isBool = false }, 
            { .optionCount = 2, .isBool = true }}
        }, { 
            .settingCount = 2,
            .settings = {{ .optionCount = 2, .isBool = true },
            { .optionCount = 2, .isBool = truee }}
        }
    } ;

    class NewSettings : public Page {
        public:
        NewSettings() = default;
        ~NewSettings() = default;

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
