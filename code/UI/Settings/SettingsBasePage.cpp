#include <UI/Settings/SettingsBasePage.hpp>
#include <UI/Settings/SettingsSubPage.hpp>
#include <UI/BMG/BMG.hpp>
#include <UI/MiscUI.hpp>

namespace CosmosUI
{
    SettingsBasePage::SettingsBasePage()
    {
        nextPageId = PAGE_NONE;
        prevPageId = SINGLE_PLAYER_MENU;

        nextMenu = MENU_NONE;

        controlSources = 2;
        externControlCount = 1;
        internControlCount = 1;
        hasBackButton = true;

        onMessageBoxClickHandler.ptmf = (&Menu::ChangeToPrevMenu);
        movieStartFrame = -1;
        extraControlNumber = 0;
        isLocked = false;
        controlCount = 0;

        onUpDownClickHandler.subject = this;
        onUpDownClickHandler.ptmf = static_cast<void (Page::*)(UpDownControl*, u32)>(&SettingsBasePage::OnUpDownClick);
        onUpDownSelectHandler.subject = this;
        onUpDownSelectHandler.ptmf = static_cast<void (Page::*)(UpDownControl*, u32)>(&SettingsBasePage::OnUpDownSelect);

        onTextChangeHandler.subject = this;
        onTextChangeHandler.ptmf = static_cast<void (Page::*)(TextUpDownValueControl::TextControl*, u32)>(&SettingsBasePage::OnTextChange);


        onButtonSelectHandler.subject = this;
        onButtonSelectHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)> (&SettingsBasePage::OnExternalButtonSelect);
        onButtonDeselectHandler.subject = this;
        onButtonDeselectHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)> (&Pages::VSSettings::OnButtonDeselect);
        onBackPressHandler.subject = this;
        onBackPressHandler.ptmf = static_cast<void (Menu::*)(u32)> (&SettingsBasePage::OnBackPress);
        onStartPressHandler.subject = this;
        onStartPressHandler.ptmf = (&Menu::HandleStartPress);


        onButtonClickHandler.subject = this;
        onButtonClickHandler.ptmf = static_cast<void (Menu::*)(PushButton *, u32)> (&SettingsBasePage::OnSaveButtonClick);

        sheetIndex = 0;
        titleBmg = 0x2A00;

        this->controlsManipulatorManager.Init(1,false);
        this->SetManipulatorManager(&controlsManipulatorManager);
        this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onBackPressHandler, false, false);
        this->controlsManipulatorManager.SetGlobalHandler(START_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onStartPressHandler, false, false);
        activePlayerBitfield = 1;
        scrollersCount = 1;
        
        this->optionsPerScroller[0] = 3; 
    }

    void SettingsBasePage::SetPrevPage(PageId pageId)
    {
        prevPageId = pageId;
        lastPage = pageId;
        return;
    }

    SettingsBasePage::~SettingsBasePage()
    {
        delete(upDownControls);
        delete(textUpDown);
    }

    void SettingsBasePage::OnInit()
    {
        upDownControls = new UpDownControl();
        textUpDown = new TextUpDownValueControl();
        currentPageId = 0;
        Menu::OnInit();
        this->SetTransitionSound(0x0,0x0);
        this->koreanChange = false;
    }

    UIControl * SettingsBasePage::CreateExternalControl(u32 id)
    {
        PushButton * button = new(PushButton);
        this->AddControl(this->controlCount, button, 0);
        this->controlCount++;
        button->Load("button", "DXInvi", "Main", this->activePlayerBitfield, 0, false);
        return button;
    }

    void SettingsBasePage::OnExternalButtonSelect(PushButton *button, u32 param_2){
        return;
    }

    void SettingsBasePage::HandleChange(u32 direction)
    {
        //this->SaveSettings();
        if(direction == 0)
        {
            upDownControls->arrowButtons[1].animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0.0f, 0);
            // upDownControls->HandleLeftPress(0, 0);
        }
        else if(direction == 2)
        {
            upDownControls->arrowButtons[0].animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0.0f, 0);
            // upDownControls->HandleRightPress(0, 0);
        }
        
        currentPageId = (currentPageId + (direction-1) + PAGECOUNT) % PAGECOUNT;
        this->upDownControls->text->SetMessage(currentPageId);

        UpDownControl::Select(upDownControls, 0);
    }

    void SettingsBasePage::OnUpDownClick(UpDownControl *upDownControl, u32 hudSlotId){
        PushButton *okButton = this->externControls[0];
        okButton->Select(0); //0 is the id
        return;
    }

    void SettingsBasePage::OnTextChange(TextUpDownValueControl::TextControl *text, u32 optionId){
        TextUpDownValueControl *valueControl = (TextUpDownValueControl*) text->parentGroup->parentControl;

        u32 bmgId = BMG_SETTINGS_PAGE + 0xB8 + optionId;// + (this->sheetIndex<<12) + optionId;
        text->SetMsgId(bmgId);

        return;
    };

    void SettingsBasePage::OnUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId){
        return;
    }

    UIControl * SettingsBasePage::CreateControl(u32 id)
    {
        if(id == 0)
        {
            UpDownControl * upDownCtrl = this->upDownControls;
            this->AddControl(this->controlCount, upDownCtrl, 0);
            this->controlCount++;

            upDownCtrl->Load(PAGECOUNT, 0, "control", "DXSettingPageUpDownBase", "UpDown4", "DXSettingPageUpDownButtonR", "RightButton",
            "DXSettingPageUpDownButtonL", "LeftButton", (UpDownDisplayedText*) this->textUpDown, 1, 0, false, true, true);
            upDownCtrl->SetOnClickHandler(&this->onUpDownClickHandler);
            upDownCtrl->SetOnSelectHandler(&this->onUpDownSelectHandler);
            upDownCtrl->id = id;

            TextUpDownValueControl * valueCtrl = this->textUpDown;

            valueCtrl->Load("control", "DXSettingPageUpDownValue", "Value", "DXSettingPageUpDownText", "Text");
            valueCtrl->SetOnTextChangeHandler(&this->onTextChangeHandler);
            upDownCtrl->SetMsgId(BMG_SETTINGS_PAGE + currentPageId);
            valueCtrl->activeTextValueControl->SetMsgId(BMG_SETTINGS_PAGE + currentPageId);
        }
        return NULL;
    }

    void SettingsBasePage::SetButtonHandlers(PushButton *button){
        PtmfHolder_2A<Page, void, PushButton *, u32> *onClickHandler = (PtmfHolder_2A<Page, void, PushButton *, u32>*) &this->onButtonClickHandler;
        button->SetOnClickHandler(onClickHandler, 0);
        button->SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*)&this->onButtonSelectHandler);
        button->SetOnDeselectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*)&this->onButtonDeselectHandler);
    }   

    int SettingsBasePage::GetActivePlayerBitfield() const{
        return this->activePlayerBitfield;
    }

    int SettingsBasePage::GetPlayerBitfield() const{
        return this->playerBitfield;
    }

    ManipulatorManager *SettingsBasePage::GetManipulatorManager(){
        return &this->controlsManipulatorManager;
    }

    int SettingsBasePage::GetRuntimeTypeInfo() const{
        return VSSettings_sInstance;
    }   

    void SettingsBasePage::OnActivate()
    {
        this->wasLanguageChanged = false;
        this->koreanChange = false;
        Menu::OnActivate();
        Page::transitionDelay = 176.0f;
        this->AddPageLayer((PageId)settingsPageIds[currentPageId] , 0);
    }

    void SettingsBasePage::OnBackPress(u32 slotId)
    {
        this->SaveSettings();
        
        using namespace CosmosData;
        u32 language = SettingsHolder::GetInstance()->GetSettings()->GetSettingValue(COSMOS_SETTING_LANGUAGE_SETTINGS);
        if(this->koreanChange) this->LoadPrevPageWithDelayById((PageId)Cosmos::WARNING_PAGE, 0.0f);
        else if(!this->wasLanguageChanged) this->LoadPrevPageWithDelayById(lastPage, 0.0f);
    }

    void SettingsBasePage::SaveSettings()
    {
        Scene * scene = MenuData::sInstance->curScene;

        using namespace CosmosData;

        for(int i = 0; i < PAGECOUNT; i++)
        {
            SettingSubPage * page = scene->Get<SettingSubPage>((PageId)(settingsPageIds[i]));
            SettingsPage * settings = &SettingsHolder::GetInstance()->GetSettings()->settings.pages[i];
            for(UpDownControl * control = page->upDownControls; control < &page->upDownControls[page->scrollersCount]; control++)
            {
                settings->setting[control->id] = control->curSelectedOption;
            }
        }
        SettingsHolder::GetInstance()->Update();
        return;
    }

    void SettingsBasePage::OnSaveButtonClick(PushButton *button, u32 hudSlotId){
        this->LoadPrevPage(button);
    }

} // namespace CosmosUI
