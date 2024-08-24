#include "Settings/UserData.hpp"
#include <UI/Settings/NewSettingsPage.hpp>
#include <Ghost/GhostManager.hpp>

#define SETTINGCONTROLCOUNT 6

namespace CosmosUI {

    void LoadInstructionText(CtrlMenuInstructionText* text, const char * folder, const char * ctrl, const char * variant){
        ControlLoader loader = ControlLoader(text);
        loader.Load(folder, ctrl,variant,nullptr);
    }

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

    void NewSettings::OnUpdate() {


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

        bool isRaceMenu = Scene::GetType(MenuData::GetStaticInstance()->curScene->menuId) == CATEGORY_GAMEPLAY;

    // Load UI Elements
        this->title.Load(false);
        
        this->pageSelector.Load(PAGE_COUNT, 0, "control", "CosmosSettingPageUpDownBase", "UpDown4", "CosmosSettingPageUpDownButtonR", "RightButton",
        "CosmosSettingPageUpDownButtonL", "LeftButton", (UpDownDisplayedText*) &this->textPageSelector, 1, 0, false, true, true);

        if(isRaceMenu)
        {
            this->backButton.Load("message_window", "Back", "ButtonBack", 1, false, true);
            LoadInstructionText(&this->bottomText, "bg", "RaceMenuObiInstructionText", "MenuObiInstructionText");

            this->textPageSelector.Load("control", "CosmosSettingPageUpDownValue", "Value_race", "CosmosSettingPageUpDownText", "Text_race");
        }
        else {
            this->backButton.Load("button", "Back", "ButtonBack", 1, false, true);
            LoadInstructionText(&this->bottomText, "bg", "MenuObiInstructionText", "MenuObiInstructionText");

            this->textPageSelector.Load("control", "CosmosSettingPageUpDownValue", "Value", "CosmosSettingPageUpDownText", "Text");
        }

        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            char variant[0x20];
            snprintf(variant, 0x20, "UpDown%d%s", i, isRaceMenu ? "_race" : "");

            this->settingSelectors[i].Load(1, 0, "control", "CosmosSettingsUpDownBase", variant, "CosmosSettingsUpDownButtonR", isRaceMenu ? "RightButton_race" : "RightButton",
            "CosmosSettingsUpDownButtonL", isRaceMenu ? "LeftButton_race" : "LeftButton", (UpDownDisplayedText*) &this->textSettingSelector[i], 1, 0, false, true, true);
            this->settingSelectors[i].id = i;

            if(isRaceMenu)
                this->textSettingSelector[i].Load("control", "CosmosSettingsUpDownValue", "Value_race", "CosmosSettingsUpDownText", "Text");
            else
                this->textSettingSelector[i].Load("control", "CosmosSettingsUpDownValue", "Value", "CosmosSettingsUpDownText", "Text");
        }

        this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, &this->onBackPressHandler, false, false);
        this->backButton.SetOnClickHandler(&this->onBackButtonPress, 0);

        this->pageSelector.SetOnClickHandler(&this->onPageClickHandler);
        this->pageSelector.SetOnSelectHandler(&this->onPageSelectHandler);
        this->pageSelector.SetOnDeselectHandler(&this->onDeselectHandler);
        this->pageSelector.zIndex = 2.0f;
        this->textPageSelector.SetOnTextChangeHandler(&this->onPageChangeHandler);

        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            UpDownControl& settingSelector = this->settingSelectors[i];
            settingSelector.SetOnClickHandler(&this->onValueSettingClickHandler);
            settingSelector.SetOnSelectHandler(&this->onValueSettingSelectHandler);
            settingSelector.SetOnDeselectHandler(&this->onDeselectHandler);
            settingSelector.SetOnChangeHandler(&this->onValueSettingChangeHandler);
            settingSelector.zIndex = 2.0f;
            settingSelector.id = i;
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
        Cosmos::Data::SettingsHolder::GetStaticInstance()->Update();
        EndStateAnimate(0.0f, 1);
    }

    void NewSettings::OnBackButtonClick(PushButton*,u32){
        Cosmos::Data::SettingsHolder::GetStaticInstance()->Update();
        EndStateAnimate(0.0f, 1);
    }

    void NewSettings::ChangePage(u8 pageId) {
        
        u8 page = Cosmos::Data::GlobalSettingsPageOrder[pageId];

        this->currentPage = page;

        Cosmos::Data::SettingPageDefinition& definition = Cosmos::Data::GlobalSettingDefinitions[page];

        this->textPageSelector.activeTextValueControl->SetMsgId(BMG_SETTING_CATEGORY + page);
        this->pageSelector.curSelectedOption = pageId;

        u32 settingCount = definition.settingCount;
        for(int i = 0; i < SETTINGCONTROLCOUNT; i++){
            UpDownControl& selector = this->settingSelectors[i];
            TextUpDownValueControl& textControl = this->textSettingSelector[i];
        
            u8 setting = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetSettingValue(page, i);
            selector.SetPlayerBitfield(i < settingCount ? 1 : 0);
            selector.isHidden = i >= settingCount;
            selector.SelectInitial(settingCount, setting);
            
            selector.SetMsgId(BMG_SETTING_FIELD + 0x100*page + 0x10*i);
            if(definition.settings[i].nameBmg != 0) selector.SetMsgId(definition.settings[i].nameBmg);

            selector.curSelectedOption = setting;
            selector.optionsCount = definition.settings[i].optionCount;

            u32 bmgId = BMG_SETTING_OPTION | ((page) << 8) | (i << 4);
            if(definition.settings[i].isBool)
                bmgId = BMG_ENABLED_DISABLED;
            else if(definition.settings[i].firstOptionBmg != 0)
                bmgId = definition.settings[i].firstOptionBmg;

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

        Cosmos::Data::SettingPageOption* def = &Cosmos::Data::GlobalSettingDefinitions[this->currentPage].settings[id];

        u32 bmg = 0;
        if(def->firstOptionBmg != 0) bmg = def->firstOptionBmg + option;
        else bmg = BMG_SETTING_OPTION + ((this->currentPage << 8) + (id << 4) + option);

        u32 bottomBmg = bmg - BMG_SETTING_OPTION + BMG_SETTING_OPTION_BOTTOM;
        if(def->firstDescBmg != 0) bottomBmg = def->firstDescBmg + option;

        if(Cosmos::Data::GlobalSettingDefinitions[this->currentPage].settings[id].isBool)
            bmg = BMG_ENABLED_DISABLED + option;
        textControl.activeTextValueControl->SetMsgId(bmg);

        Cosmos::Data::SettingsHolder::GetStaticInstance()->SetSettingValue(option, this->currentPage, id); 
        SettingsValueUpdateHook::exec(this->currentPage * 8 + id);
        this->bottomText.SetMsgId(bottomBmg);

    }
    void NewSettings::OnValueControlClick(UpDownControl* upDownControl, u32 hudSlotId) {}
    void NewSettings::OnValueControlSelect(UpDownControl* upDownControl, u32 hudSlotId) {
        u32 bmg = BMG_SETTING_OPTION_BOTTOM + ((this->currentPage << 8) + (upDownControl->id << 4) + upDownControl->curSelectedOption);
        Cosmos::Data::SettingPageDefinition& definition = Cosmos::Data::GlobalSettingDefinitions[this->currentPage];
        if(definition.settings[upDownControl->id].firstDescBmg != 0)
            bmg = definition.settings[upDownControl->id].firstDescBmg + upDownControl->curSelectedOption;
        this->bottomText.SetMsgId(bmg);
    }


}
