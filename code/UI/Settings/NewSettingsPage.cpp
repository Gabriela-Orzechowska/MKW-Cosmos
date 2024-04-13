#include <UI/Settings/NewSettingsPage.hpp>

#define SETTINGCONTROLCOUNT 6

namespace CosmosUI {

    NewSettings::NewSettings() {
        SetupHandler(onBackPressHandler, void (Page::*)(u32), &NewSettings::OnBack);
        SetupHandler(onBackButtonPress, void (Page::*)(PushButton*,u32), &NewSettings::OnBackButtonClick);

        SetupHandler(onPageChangeHandler, void (Page::*)(TextUpDownValueControl::TextControl*, u32), &NewSettings::OnSettingsPageControlChange);
        SetupHandler(onPageClickHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnSettingsPageControlClick);
        SetupHandler(onPageSelectHandler, void (Page::*)(UpDownControl*,u32), &NewSettings::OnSettingsPageControlSelect);
        
        SetupHandler(onValueSettingChangeHandler, void (Page::*)(UpDownControl*,u32,u32), &NewSettings::OnValueControlChange);
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
        this->pageSelector.drawPriority = -1.0f;
        this->textPageSelector.SetOnTextChangeHandler(&this->onPageChangeHandler);

        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            this->settingSelectors[i].SetOnClickHandler(&this->onValueSettingClickHandler);
            this->settingSelectors[i].SetOnSelectHandler(&this->onValueSettingSelectHandler);
            this->settingSelectors[i].SetOnDeselectHandler(&this->onDeselectHandler);
            this->settingSelectors[i].SetOnChangeHandler(&this->onValueSettingChangeHandler);
            this->settingSelectors[i].id = i;
        }

        this->title.SetMsgId(0x0);
        this->pageSelector.SelectDefault(0);
        this->currentPage = 0;
    }   

    void NewSettings::OnActivate() {
        this->bottomText.SetMsgId(0x0);
        this->title.SetMsgId(BMG_SETTINGS_TITLE);
        ChangePage(this->currentPage);
    }

    void NewSettings::BeforeEntranceAnimations() {
        UpDownControl::Select(&this->pageSelector, 0);
        ChangePage(this->currentPage);
    }

    void NewSettings::OnBack(u32 hudSlotId) {
        CosmosData::SettingsHolder::GetInstance()->Update();
        EndStateAnimate(0.0f, 1);
    }

    void NewSettings::OnBackButtonClick(PushButton*,u32){
        CosmosData::SettingsHolder::GetInstance()->Update();
        EndStateAnimate(0.0f, 1);
    }

    void NewSettings::ChangePage(u8 page) {
        
        this->currentPage = page;

        CosmosData::SettingPageDefinition& definition = CosmosData::GlobalSettingDefinitions[page];

        this->textPageSelector.activeTextValueControl->SetMsgId(BMG_SETTING_CATEGORY + page);
        this->pageSelector.curSelectedOption = page;

        u32 settingCount = definition.settingCount;
        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            UpDownControl& selector = this->settingSelectors[i];
            TextUpDownValueControl& textControl = this->textSettingSelector[i];
        
            u8 setting = CosmosData::SettingsHolder::GetInstance()->GetSettingValue(page, i);
            selector.SetPlayerBitfield(i < settingCount ? 1 : 0);
            selector.isHidden = i >= settingCount;
            selector.SelectInitial(settingCount, setting);
            
            selector.SetMsgId(BMG_SETTING_FIELD + 0x100*page + 0x10*i);

            selector.curSelectedOption = setting;
            selector.optionsCount = definition.settings[i].optionCount;

            u32 bmgId = BMG_SETTING_OPTION | ((page) << 8) | (i << 4);
            if(definition.settings[i].isBool)
                bmgId = BMG_ENABLED_DISABLED;
            bmgId += setting;
            textControl.activeTextValueControl->SetMsgId(bmgId);
        }
        this->bottomText.SetMsgId(BMG_SETTINGS_PAGE_BOTTOM + pageSelector.curSelectedOption);
    }

    void NewSettings::OnSettingsPageControlChange(TextUpDownValueControl::TextControl* valueControl, u32 optionId) {
        ChangePage(optionId);
    }
    void NewSettings::OnSettingsPageControlClick(UpDownControl* upDownControl, u32 hudSlotId) {}
    void NewSettings::OnSettingsPageControlSelect(UpDownControl* upDownControl, u32 hudSlotId) {
        this->bottomText.SetMsgId(BMG_SETTINGS_PAGE_BOTTOM + upDownControl->curSelectedOption);
    }

    void NewSettings::OnValueControlChange(UpDownControl* control, u32 hudSlotId, u32 option) {
        // u32 bmgId = BMG_SETTING_OPTION + ((this->currentPage) << 8) + (currentTextControlIndex << 4) + optionId;
        u32 id = control->id;
        TextUpDownValueControl& textControl = this->textSettingSelector[id];

        u32 bmg = BMG_SETTING_OPTION + ((this->currentPage << 8) + (id << 4) + option);
        u32 bottomBmg = bmg - BMG_SETTING_OPTION + BMG_SETTING_OPTION_BOTTOM;
        if(CosmosData::GlobalSettingDefinitions[this->currentPage].settings[id].isBool)
            bmg = BMG_ENABLED_DISABLED + option;
        textControl.activeTextValueControl->SetMsgId(bmg);

        

        CosmosData::SettingsHolder::GetInstance()->SetSettingValue(option, this->currentPage, id); 
        this->bottomText.SetMsgId(bottomBmg);

    }
    void NewSettings::OnValueControlClick(UpDownControl* upDownControl, u32 hudSlotId) {}
    void NewSettings::OnValueControlSelect(UpDownControl* upDownControl, u32 hudSlotId) {
        u32 bmg = BMG_SETTING_OPTION_BOTTOM + ((this->currentPage << 8) + (upDownControl->id << 4) + upDownControl->curSelectedOption);
        this->bottomText.SetMsgId(bmg);
    }




}
