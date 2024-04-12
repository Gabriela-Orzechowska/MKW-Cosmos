#include <UI/Settings/NewSettingsPage.hpp>

#define SETTINGCONTROLCOUNT 5


namespace CosmosUI {
    void NewSettings::OnInit() {

    // Init Manipulator
        controlsManipulatorManager.Init(1, false);
        this->SetManipulatorManager(&controlsManipulatorManager);
        controlsManipulatorManager.SetDistanceFunc(2);

    // Init Controls
        this->InitControlGroup(4 + SETTINGCONTROLCOUNT);
        this->AddControl(0, &title, false);
        this->AddControl(1, &bottomText, false);
        this->AddControl(2, &backButton, false);
        this->AddControl(3, &pageSelector, false);
        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            this->AddControl(4 + i, &settingSelectors[i], false);
        }

    // Load UI Elements
        title.Load(false);
        pageSelector.Load(PAGE_COUNT, 0, "control", "DXSettingPageUpDownBase", "UpDown4", "DXSettingPageUpDownButtonR", "RightButton",
            "DXSettingPageUpDownButtonL", "LeftButton", (UpDownDisplayedText*) &textPageSelector, 1, 0, false, true, true);

        textPageSelector.Load("control", "DXSettingPageUpDownValue", "Value", "DXSettingPageUpDownText", "Text");

        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            char variant[0x20];
            snprintf(variant, 0x20, "UpDown%d", i);

            settingSelectors[i].Load(1, 0, "control", "DXSettingsUpDownBase", variant, "DXSettingsUpDownButtonR", "RightButton",
            "DXSettingsUpDownButtonL", "LeftButton", (UpDownDisplayedText*) &textSettingSelector[i], 1, 0, false, true, true);
        }

        if(Scene::GetType(MenuData::GetStaticInstance()->curScene->menuId) == CATEGORY_GAMEPLAY){
            backButton.Load("message_window", "Back", "ButtonBack", 1, false, true);
        } else {
            backButton.Load("button", "Back", "ButtonBack", 1, false, true);
        }
        bottomText.Load();

        controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, &onBackPressHandler, false, false);
        backButton.SetOnClickHandler(&onBackButtonPress, 0);

        pageSelector.SetOnClickHandler(&onPageClickHandler);
        pageSelector.SetOnSelectHandler(&onPageSelectHandler);
        textPageSelector.SetOnTextChangeHandler(&onPageChangeHandler);

        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            settingSelectors[i].SetOnClickHandler(&onValueSettingClickHandler);
            settingSelectors[i].SetOnSelectHandler(&onValueSettingSelectHandler);
            textSettingSelector[i].SetOnTextChangeHandler(&onValueSettingChangeHandler);
        }

    }   

    void NewSettings::OnActivate() {
        // UpDownControl::Select(&pageSelector, 0);
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
