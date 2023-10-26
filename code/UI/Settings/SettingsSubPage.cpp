#include <UI/Settings/SettingsSubPage.hpp>
#include <UI/BMG/BMG.hpp>

namespace DXUI
{
    SettingSubPage::SettingSubPage(SettingPageDefinition * definition, u32 pageId)
    {
        nextPageId = PAGE_NONE;
        prevPageId = PAGE_NONE;

        nextMenu = MENU_NONE;

        controlSources = 2;
        externControlCount = 0;
        internControlCount = definition->settingCount + 1;
        hasBackButton = true;

        onMessageBoxClickHandler.ptmf = (&Menu::ChangeToPrevMenu);
        movieStartFrame = -1;
        extraControlNumber = 0;
        isLocked = false;
        controlCount = 0;

        onUpDownClickHandler.subject = this;
        onUpDownClickHandler.ptmf = static_cast<void (Page::*)(UpDownControl*, u32)>(&SettingSubPage::OnUpDownClick);
        onUpDownSelectHandler.subject = this;
        onUpDownSelectHandler.ptmf = static_cast<void (Page::*)(UpDownControl*, u32)>(&SettingSubPage::OnUpDownSelect);


        //Dummy
        onMainUpDownSelectHandler.subject = this;
        onMainUpDownSelectHandler.ptmf = static_cast<void (Page::*)(UpDownControl*, u32)>(&SettingSubPage::OnMainUpDownSelect);
        onMainUpDownDeselectHandler.subject = this;
        onMainUpDownDeselectHandler.ptmf = static_cast<void (Page::*)(UpDownControl*, u32)>(&SettingSubPage::OnMainUpDownDeselect);
        onMainUpDownChangeHandler.subject = this;
        onMainUpDownChangeHandler.ptmf = static_cast<void (Page::*)(UpDownControl*, u32, u32)>(&SettingSubPage::OnMainUpDownChange);

        onTextChangeHandler.subject = this;
        onTextChangeHandler.ptmf = static_cast<void (Page::*)(TextUpDownValueControl::TextControl*, u32)>(&SettingSubPage::OnTextChange);


        onButtonSelectHandler.subject = this;
        onButtonSelectHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)> (&SettingSubPage::OnExternalButtonSelect);
        onButtonDeselectHandler.subject = this;
        onButtonDeselectHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)> (&Pages::VSSettings::OnButtonDeselect);
        onBackPressHandler.subject = this;
        onBackPressHandler.ptmf = static_cast<void (Menu::*)(u32)> (&SettingSubPage::OnBackPress);
        onStartPressHandler.subject = this;
        onStartPressHandler.ptmf = (&Menu::HandleStartPress);


        onButtonClickHandler.subject = this;
        onButtonClickHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)> (&SettingSubPage::OnSaveButtonClick);

        pageIndex = pageId;
        titleBmg = 0x00;

        this->controlsManipulatorManager.Init(1,false);
        this->SetManipulatorManager(&controlsManipulatorManager);
        this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onBackPressHandler, false, false);
        this->controlsManipulatorManager.SetGlobalHandler(START_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onStartPressHandler, false, false);
        activePlayerBitfield = 1;
        scrollersCount = definition->settingCount;
        
        memcpy(&pageDefinition, definition, sizeof(SettingPageDefinition));
    }

    SettingSubPage::~SettingSubPage()
    {
        delete[] upDownControls;
        delete[] textUpDownPlus;
    }

    void SettingSubPage::OnInit()
    {
        upDownControls = new UpDownControl[this->scrollersCount+1];
        textUpDownPlus = new TextUpDownValueControlPlus[this->scrollersCount+1];
        Menu::OnInit();
        this->SetTransitionSound(0x0,0x0);
        //UpDownControl::Select(&this->upDownControls[0], 0);
    }

    UIControl * SettingSubPage::CreateExternalControl(u32 id)
    {
        return NULL;
    }

    void SettingSubPage::OnExternalButtonSelect(PushButton *button, u32 param_2){
        return;
    }

    void SettingSubPage::OnUpDownClick(UpDownControl *upDownControl, u32 hudSlotId){
        //PushButton *okButton = this->externControls[0];
        //okButton->Select(0); //0 is the id
        return;
    }

    void SettingSubPage::OnTextChange(TextUpDownValueControl::TextControl *text, u32 optionId){
        TextUpDownValueControlPlus *valueControl = (TextUpDownValueControlPlus*) text->parentGroup->parentControl;

        if(valueControl->id == 99)
        {
            text->SetMsgId(0x0);
            this->bottomText->SetMsgId(0x00);
            return;
        }

        u32 bmgId = BMG_SETTING_OPTION + ((pageIndex-MINPAGE) << 8) + (valueControl->id << 4) + optionId;

        u32 bottomBmgId = bmgId - BMG_SETTING_OPTION + BMG_SETTING_OPTION_BOTTOM;

        if(pageDefinition.settings[valueControl->id].isBool)
            bmgId = BMG_ENABLED_DISABLED + optionId;

        text->SetMsgId(bmgId);
        this->bottomText->SetMsgId(bottomBmgId);
        //TEXT HERE 80853a10
        return;
    };

    void SettingSubPage::OnUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId){
        u32 bmgId = BMG_SETTING_OPTION_BOTTOM + ((pageIndex-MINPAGE) << 8) + (upDownControl->id << 4) + upDownControl->curSelectedOption;

        this->bottomText->SetMsgId(bmgId);
        return;
    }


    void SettingSubPage::OnMainUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId){
        UpDownControl * baseControl = this->basePage->upDownControls;

        baseControl->HandleSelect(hudSlotId, 0);
        this->bottomText->SetMsgId(BMG_SETTINGS_PAGE_BOTTOM + MINPAGE + baseControl->curSelectedOption);

        return;
    }

    void SettingSubPage::OnMainUpDownDeselect(UpDownControl *upDownControl, u32 hudSlotId){
        UpDownControl * baseControl = this->basePage->upDownControls;

        baseControl->HandleButtonDeselect(hudSlotId, 0);

        return;
    }

    void SettingSubPage::OnMainUpDownChange(UpDownControl *upDownControl, u32 hudSlotId, u32 optionId){
        
        this->basePage->HandleChange(optionId);

        PageId id = (PageId)this->basePage->currentPageId;

        if(optionId == 0)
            this->LoadPrevPageWithDelayById(id, 0.0f);
        else
            this->LoadNextPageWithDelayById(id, 0.0f);
        
        upDownControl->curSelectedOption = 1;

        return;
    }


    UIControl * SettingSubPage::CreateControl(u32 id)
    {
        DXData::SettingsHolder * settingsHolder = DXData::SettingsHolder::GetInstance();
        if(id < this->scrollersCount)
        {
            UpDownControl * upDownCtrl = &this->upDownControls[id];
            this->AddControl(this->controlCount, upDownCtrl, 0);
            this->controlCount++;

            //char * variant = "UpDown0";
            char variant[0x20];
            snprintf(variant, 0x20, "UpDown%d", id);

            upDownCtrl->Load(this->pageDefinition.settings[id].optionCount, settingsHolder->GetSettings()->pages[this->pageId - MINPAGE].setting[id], "control", "DXSettingsUpDownBase", variant, "DXSettingsUpDownButtonR", "RightButton",
            "DXSettingsUpDownButtonL", "LeftButton", (UpDownDisplayedText*) &this->textUpDownPlus[id], 1, 0, false, true, true);
            upDownCtrl->SetOnClickHandler(&this->onUpDownClickHandler);
            upDownCtrl->SetOnSelectHandler(&this->onUpDownSelectHandler);
            upDownCtrl->id = id;

            TextUpDownValueControlPlus * valueCtrl = &this->textUpDownPlus[id];

            valueCtrl->Load("control", "DXSettingsUpDownValue", "Value", "DXSettingsUpDownText", "Text");
            valueCtrl->SetOnTextChangeHandler(&this->onTextChangeHandler);
            valueCtrl->id = id;

            u32 bmgOption = BMG_SETTING_FIELD | ((pageIndex-MINPAGE) << 8) | (id << 4);
            OSReport("[DX] BMG ID: %d", bmgOption);

            u32 bmgId = BMG_SETTING_OPTION | ((pageIndex-MINPAGE) << 8) | (id << 4);
            if(this->pageDefinition.settings[id].isBool)
                bmgId = BMG_ENABLED_DISABLED;

            upDownCtrl->SetMsgId(bmgOption);
            valueCtrl->activeTextValueControl->SetMsgId(bmgId);
        }

        else if(id = this->scrollersCount)
        {
            UpDownControl * upDownCtrl = &this->upDownControls[id];
            this->mainControlId = id;
            this->AddControl(this->controlCount, upDownCtrl, 0);
            this->controlCount++;

            upDownCtrl->Load(3, 1, "control", "DXSettingPageUpDownBase", "UpDownTrans", "DXSettingPageUpDownButtonR", "RightButtonTrans",
            "DXSettingPageUpDownButtonL", "LeftButtonTrans", (UpDownDisplayedText*) &this->textUpDownPlus[id], 1, 0, false, true, true);
            upDownCtrl->SetOnClickHandler(&this->onUpDownClickHandler);
            upDownCtrl->SetOnSelectHandler(&this->onMainUpDownSelectHandler);
            upDownCtrl->SetOnDeselectHandler(&this->onMainUpDownDeselectHandler);
            upDownCtrl->SetOnChangeHandler(&this->onMainUpDownChangeHandler);
            upDownCtrl->id = 99;

            TextUpDownValueControlPlus * valueCtrl = &this->textUpDownPlus[id];

            valueCtrl->Load("control", "DXSettingPageUpDownValue", "ValueTrans", "DXSettingPageUpDownText", "TextTrans");
            valueCtrl->SetOnTextChangeHandler(&this->onTextChangeHandler);
            valueCtrl->id = 99;
            upDownCtrl->SetMsgId(0x00);
            valueCtrl->activeTextValueControl->SetMsgId(0x00);
        }

        return NULL;
    }

    void SettingSubPage::SetButtonHandlers(PushButton *button){
        PtmfHolder_2A<Page, void, PushButton *, u32> *onClickHandler = (PtmfHolder_2A<Page, void, PushButton *, u32>*) &this->onButtonClickHandler;
        button->SetOnClickHandler(onClickHandler, 0);
        button->SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*)&this->onButtonSelectHandler);
        button->SetOnDeselectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*)&this->onButtonDeselectHandler);
    }   

    int SettingSubPage::GetActivePlayerBitfield() const{
        return this->activePlayerBitfield;
    }

    int SettingSubPage::GetPlayerBitfield() const{
        return this->playerBitfield;
    }

    ManipulatorManager *SettingSubPage::GetManipulatorManager(){
        return &this->controlsManipulatorManager;
    }

    int SettingSubPage::GetRuntimeTypeInfo() const{
        return VSSettings_sInstance;
    }   

    void SettingSubPage::OnActivate()
    {
        DXData::SettingsPage * settingsPage = &DXData::SettingsHolder::GetInstance()->GetSettings()->pages[this->pageId - MINPAGE];
        UpDownControl::Select(&this->upDownControls[this->mainControlId], 0);
        basePage = MenuData::sInstance->curScene->Get<SettingsBasePage>((PageId)DX::SETTINGS_MAIN);
        //this->basePage->upDownControls->HandleButtonDeselect(0,0);
        nextPageId = PAGE_NONE;
        prevPageId = basePage->lastPage;
        backButton.isHidden = true;  

        //for(UpDownControl * control = this->upDownControls; control < &this->upDownControls[this->scrollersCount]; control++)
        //{
        //    control->curSelectedOption = settingsPage->setting[control->id];
        //}

        Menu::OnActivate();
    }

    void SettingSubPage::OnBackPress(u32 slotId)
    {
        EndStateAnimate(0.0f, 1);
        basePage->OnBackPress(slotId);
    }

    void SettingSubPage::OnSaveButtonClick(PushButton *button, u32 hudSlotId){
        //this->LoadPrevPage(button);
    }

    void TextUpDownValueControlPlus::SetMessage(u32 optionId){
        TextUpDownValueControl::TextControl *text = this->activeTextValueControl;
        if(this->id == 99)
        {
            text->SetMsgId(0x0);
            return;
        }
        
        SettingSubPage * basePage = ((DXUI::SettingSubPage *)this->parentGroup->parentPage);

        u32 bmgId = BMG_SETTING_OPTION + ((basePage->pageId-MINPAGE) << 8) + (id << 4) + optionId;
        if(basePage->pageDefinition.settings[id].isBool)
           bmgId = BMG_ENABLED_DISABLED | optionId;
        text->SetMsgId(bmgId);
    }

} // namespace DXUI
