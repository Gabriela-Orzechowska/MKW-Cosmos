#include <UI/Settings/NewSettingsPage.hpp>

#define SETTINGCONTROLCOUNT 6

namespace CosmosUI {

    NewSettings::NewSettings() {
        CosmosLog("This address: %p\n", this);

        SetupHandler(onBackPressHandler, void (Page::*)(u32), &NewSettings::OnBack);
        SetupHandler(onBackButtonPress, void (Page::*)(PushButton*,u32), &NewSettings::OnBackButtonClick);

        SetupHandler(onPageChangeHandler, void (Page::*)(TextUpDownValueControl::TextControl*, u32), &NewSettings::OnSettingsPageControlChange);
        SetupHandler(onPageClickHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnSettingsPageControlClick);
        SetupHandler(onPageSelectHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnSettingsPageControlSelect);
        
        SetupHandler(onValueSettingChangeHandler, void (Page::*)(TextUpDownValueControl::TextControl*,u32), &NewSettings::OnValueControlChange);
        SetupHandler(onValueSettingClickHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnValueControlClick);
        SetupHandler(onValueSettingSelectHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnValueControlSelect);
        SetupHandler(onDeselectHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnDummyDeselect);
    }

    void NewSettings::OnInit() {
    // Init Manipulator
        this->controlsManipulatorManager.Init(0x1, false);
        this->controlsManipulatorManager.SetDistanceFunc(2);
        this->SetManipulatorManager(&this->controlsManipulatorManager);
        
    // Init Controls
        this->InitControlGroup(4 + SETTINGCONTROLCOUNT);
        this->AddControl(0, &this->backButton, false);
        this->AddControl(1, &this->bottomText, false);
        this->AddControl(2, &this->title, false);
        this->AddControl(3, &this->pageSelector, false);
        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            this->AddControl(4 + i, &this->settingSelectors[i], false);
        }

    // Load UI Elements
        this->title.Load(false);
        this->bottomText.Load();
        this->pageSelector.Load(PAGE_COUNT, 0, "control", "DXSettingPageUpDownBase", "UpDown4", "DXSettingPageUpDownButtonR", "RightButton",
            "DXSettingPageUpDownButtonL", "LeftButton", (UpDownDisplayedText*) &this->textPageSelector, 1, 0, false, true, true);

        this->textPageSelector.Load("control", "DXSettingPageUpDownValue", "Value", "DXSettingPageUpDownText", "Text");

        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            char variant[0x20];
            snprintf(variant, 0x20, "UpDown%d", i);

            this->settingSelectors[i].Load(1, 0, "control", "DXSettingsUpDownBase", variant, "DXSettingsUpDownButtonR", "RightButton",
            "DXSettingsUpDownButtonL", "LeftButton", (UpDownDisplayedText*) &this->textSettingSelector[i], 1, 0, false, true, true);
            this->settingSelectors[i].id = i;

            this->textSettingSelector[i].Load("control", "DXSettingsUpDownValue", "Value", "DXSettingsUpDownText", "Text");
        }

        this->backButton.Load("button", "Back", "ButtonBack", 1, false, true);

        this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, &this->onBackPressHandler, false, false);
        this->backButton.SetOnClickHandler(&this->onBackButtonPress, 0);

        this->pageSelector.SetOnClickHandler(&this->onPageClickHandler);
        this->pageSelector.SetOnSelectHandler(&this->onPageSelectHandler);
        this->pageSelector.SetOnDeselectHandler(&this->onDeselectHandler);
        this->textPageSelector.SetOnTextChangeHandler(&this->onPageChangeHandler);

        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            this->settingSelectors[i].SetOnClickHandler(&this->onValueSettingClickHandler);
            this->settingSelectors[i].SetOnSelectHandler(&this->onValueSettingSelectHandler);
            this->settingSelectors[i].SetOnDeselectHandler(&this->onDeselectHandler);
            this->textSettingSelector[i].SetOnTextChangeHandler(&this->onValueSettingChangeHandler);
        }

        this->title.SetMsgId(0x0);
        this->pageSelector.SelectDefault(0);
    }   

    void NewSettings::OnActivate() {
        CosmosLog("I tried so hard, and got so far");
        this->bottomText.SetMsgId(0x0);
    }

    void NewSettings::OnBack(u32 hudSlotId) {
        EndStateAnimate(0.0f, 0);
    }

    void NewSettings::OnBackButtonClick(PushButton*,u32){
        EndStateAnimate(0.0f,0);
    }

    void NewSettings::OnSettingsPageControlChange(TextUpDownValueControl::TextControl* valueControl, u32 hudSlotId) {}
    void NewSettings::OnSettingsPageControlClick(UpDownControl* upDownControl, u32 hudSlotId) {}
    void NewSettings::OnSettingsPageControlSelect(UpDownControl* upDownControl, u32 hudSlotId) {}

    void NewSettings::OnValueControlChange(TextUpDownValueControl::TextControl* valueControl, u32 hudSlotId) {}
    void NewSettings::OnValueControlClick(UpDownControl* upDownControl, u32 hudSlotId) {}
    void NewSettings::OnValueControlSelect(UpDownControl* upDownControl, u32 hudSlotId) {}


}
