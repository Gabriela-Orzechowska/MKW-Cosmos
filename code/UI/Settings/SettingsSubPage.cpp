#include <UI/Settings/SettingsSubPage.hpp>

namespace DXUI
{
    SettingSubPage::SettingSubPage(u32 selectorCount, const u8 optionCount[8], u32 sheetId)
    {
        nextPageId = PAGE_NONE;
        prevPageId = OPTIONS;

        nextMenu = MENU_NONE;

        controlSources = 2;
        externControlCount = 0;
        internControlCount = selectorCount + 1;
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

        sheetIndex = sheetId;
        titleBmg = 0x2810;

        this->controlsManipulatorManager.Init(1,false);
        this->SetManipulatorManager(&controlsManipulatorManager);
        this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onBackPressHandler, false, false);
        this->controlsManipulatorManager.SetGlobalHandler(START_PRESS, (PtmfHolder_1A<Page, void, u32>*) &onStartPressHandler, false, false);
        activePlayerBitfield = 1;
        scrollersCount = selectorCount;
        
        memcpy(this->optionsPerScroller, optionCount, 8);
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

    kmWrite32(0x805fd9f4, 0x380000b7);

    void SettingSubPage::OnUpDownClick(UpDownControl *upDownControl, u32 hudSlotId){
        //PushButton *okButton = this->externControls[0];
        //okButton->Select(0); //0 is the id
        return;
    }

    void SettingSubPage::OnTextChange(TextUpDownValueControl::TextControl *text, u32 optionId){
        TextUpDownValueControlPlus *valueControl = (TextUpDownValueControlPlus*) text->parentGroup->parentControl;

        u32 bmgId = 0x2800;
        text->SetMsgId(bmgId);
        this->bottomText->SetMsgId(bmgId + (valueControl->id+1<<8));
        //TEXT HERE 80853a10
        return;
    };

    void SettingSubPage::OnUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId){
        u32 bmgId = 0x2801;
        this->bottomText->SetMsgId(bmgId);
        return;
    }

    UIControl * SettingSubPage::CreateControl(u32 id)
    {
        if(id < this->scrollersCount)
        {
            UpDownControl * upDownCtrl = &this->upDownControls[id];
            this->AddControl(this->controlCount, upDownCtrl, 0);
            this->controlCount++;

            //char * variant = "UpDown0";
            char variant[0x20];
            snprintf(variant, 0x20, "UpDown%d", id);

            upDownCtrl->Load(this->optionsPerScroller[id], 0, "control", "DXSettingsUpDownBase", variant, "DXSettingsUpDownButtonR", "RightButton",
            "DXSettingsUpDownButtonL", "LeftButton", (UpDownDisplayedText*) &this->textUpDownPlus[id], 1, 0, false, true, true);
            upDownCtrl->SetOnClickHandler(&this->onUpDownClickHandler);
            upDownCtrl->SetOnSelectHandler(&this->onUpDownSelectHandler);
            upDownCtrl->id = id;

            TextUpDownValueControlPlus * valueCtrl = &this->textUpDownPlus[id];

            valueCtrl->Load("control", "DXSettingsUpDownValue", "Value", "DXSettingsUpDownText", "Text");
            valueCtrl->SetOnTextChangeHandler(&this->onTextChangeHandler);
            valueCtrl->id = id;
            u32 bmgCategory = 0x6000 + (this->sheetIndex<<12);
            upDownCtrl->SetMsgId(0x7008);
            valueCtrl->activeTextValueControl->SetMsgId(0x7000);
        }

        else if(id = this->scrollersCount)
        {
            UpDownControl * upDownCtrl = &this->upDownControls[id];
            this->AddControl(this->controlCount, upDownCtrl, 0);
            this->controlCount++;

            //char * variant = "UpDown0";
            char variant[0x20];
            snprintf(variant, 0x20, "UpDown%d", id);

            upDownCtrl->Load(this->optionsPerScroller[id], 0, "control", "DXSettingPageUpDownBase", "UpDownTrans", "DXSettingPageUpDownButtonR", "RightButtonTrans",
            "DXSettingPageUpDownButtonL", "LeftButtonTrans", (UpDownDisplayedText*) &this->textUpDownPlus[id], 1, 0, false, true, true);
            upDownCtrl->SetOnClickHandler(&this->onUpDownClickHandler);
            upDownCtrl->SetOnSelectHandler(&this->onUpDownSelectHandler);
            upDownCtrl->id = id;

            TextUpDownValueControlPlus * valueCtrl = &this->textUpDownPlus[id];

            valueCtrl->Load("control", "DXSettingPageUpDownValue", "ValueTrans", "DXSettingPageUpDownText", "TextTrans");
            valueCtrl->SetOnTextChangeHandler(&this->onTextChangeHandler);
            valueCtrl->id = id;
            u32 bmgCategory = 0x6000 + (this->sheetIndex<<12);
            upDownCtrl->SetMsgId(0x00);
            valueCtrl->activeTextValueControl->SetMsgId(0x700a);
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
        UpDownControl::Select(&this->upDownControls[0], 0);
        Menu::OnActivate();
    }

    void SettingSubPage::OnBackPress(u32 slotId)
    {
        this->EndStateAnimate(0.0f, 1);
        MenuData::sInstance->curScene->Get<SettingsBasePage>((PageId)DX::SET_PAGE_1)->OnBackPress(slotId);
    }

    void SettingSubPage::OnSaveButtonClick(PushButton *button, u32 hudSlotId){
        this->LoadPrevPage(button);
    }

    void TextUpDownValueControlPlus::SetMessage(u32 optionId){
        TextUpDownValueControl::TextControl *text = this->activeTextValueControl;
        u32 bmgId = 0x5000 + optionId;
        text->SetMsgId(bmgId);
    }

} // namespace DXUI
