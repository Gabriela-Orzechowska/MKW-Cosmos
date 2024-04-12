#include <UI/CupSelect/CupSelect.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuCourse.hpp>
#include <SlotExpansion/CupManager.hpp>

namespace CosmosUI
{
    void ExtendCupSelectCupInitSelf(CtrlMenuCupSelectCup * cups);
    static s32 lefttemp = 0;


    CupSelectPlus * CreateCupPage()
    {
        CupSelectPlus * cup = new(CupSelectPlus);
        return cup;
    };

    kmWrite32(0x80623d98, 0x60000000);
    kmCall(0x80623da4, CreateCupPage);

    CupSelectPlus::CupSelectPlus()
    {
        internControlCount += 1;
        onRightArrowSelectHandler.subject = this;
        onRightArrowSelectHandler.ptmf = &CupSelectPlus::OnRightArrowSelect;
        onLeftArrowSelectHandler.subject = this;
        onLeftArrowSelectHandler.ptmf = &CupSelectPlus::OnLeftArrowSelect;
        
        this->ctrlMenuCupSelectCup.curCupID = Cosmos::CupManager::sInstance->lastSelectedCup;
        onSwitchPressHandler.subject = this;
        onSwitchPressHandler.ptmf = &CupSelectPlus::OnSwitchPress;
        this->controlsManipulatorManager.SetGlobalHandler(SWITCH_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onSwitchPressHandler, false, false);
    }
    kmWrite32(0x80841244, 0x38800001);
    
    kmWrite32(0x808a85d1, 0x44584353);
    
    UIControl *  CupSelectPlus::CreateControl(u32 controlId)
    {
        switch(controlId){
            case 2:
                this->AddControl(2, &this->arrows, 0);
                arrows.SetRightArrowHandler((PtmfHolder_2A<Page, void, SheetSelectControl *, u32>*)&this->onRightArrowSelectHandler);
                arrows.SetLeftArrowHandler((PtmfHolder_2A<Page, void, SheetSelectControl *, u32>*)&this->onLeftArrowSelectHandler);
                arrows.Load("button", "DXCupSelectRightArrow", "ButtonArrowRight", "DXCupSelectLeftArrow", "ButtonArrowLeft", 1, 0, false);
                this->controlCount++;
                return &this->arrows;
            default:
                return CupSelect::CreateControl(controlId);
        }
    }

    void CupSelectPlus::OnRightArrowSelect(SheetSelectControl::SheetSelectButton * button, u32 slotId){
        this->ScrollCups(1);
    }

    void CupSelectPlus::OnLeftArrowSelect(SheetSelectControl::SheetSelectButton * button, u32 slotId){
        this->ScrollCups(-1);
    }

    void CupSelectPlus::ChangeTPL(LayoutUIControl * ctrl, u32 id)
    {
        char tpl[0x20];
        snprintf(tpl, 0x20, "icon_cup_%03d.tpl", id);
        CosmosUI::ChangePaneImage(ctrl, "icon", tpl);
        CosmosUI::ChangePaneImage(ctrl, "icon_light_01", tpl);
        CosmosUI::ChangePaneImage(ctrl, "icon_light_02", tpl);
    }

    void CupSelectPlus::ScrollCups(s32 direction)
    {
        u32 CupCount = Cosmos::CupManager::sInstance->GetCupCount();
        CtrlMenuCupSelectCup *cupCtrl = &this->ctrlMenuCupSelectCup;
        cupCtrl->curCupID = (cupCtrl->curCupID + (direction * 2) + CupCount) % CupCount;
        this->ctrlMenuCupSelectCourse.UpdateTrackList(cupCtrl->curCupID);
        for(int i = 0; i < 8; i++)
        {
            PushButton * button = &this->ctrlMenuCupSelectCup.cupButtons[i];
            u32 id = button->buttonId;
            id = (id + (direction * 2) + CupCount) % CupCount;
            button->SetMsgId(id + BMG_CUPS);
            button->buttonId = id;
            this->ChangeTPL(button, id);
        }
    }

    void CupSelectPlus::OnInit()
    {
        Pages::CupSelect::OnInit();
        for(int i = 0; i < 8; i++)
        {
            u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
            this->ctrlMenuCupSelectCup.cupButtons[i].buttonId = id;
        }
        return;
    }

    void CupSelectPlus::OnActivate()
    {
        this->ctrlMenuCupSelectCup.curCupID = Cosmos::CupManager::sInstance->lastSelectedCup;
        Pages::CupSelect::OnActivate();
        lefttemp = 0;
        this->bottomText->SetMsgId(0x2810 + (u32)currentLayout);
        //ExtendCupSelectCupInitSelf(&this->ctrlMenuCupSelectCup);
    }

    void CupSelectPlus::OnSwitchPress(u32 slotId)
    {        
        currentLayout = static_cast<CUP_LAYOUT>(((u32)currentLayout) ^ 0x1);

        DVDFileInfo fileHandle;
        char * filepath = "/bin/cup2.bin";

        char buffer[0x20] __attribute__ ((aligned(0x20)));

        
        if(((u32)currentLayout) & 0x1)
        {
            PageId lastPage = this->prevPageId;
            this->LoadNextPageWithDelayById(CUP_SELECT, 0.0f);
            this->prevPageId = lastPage;
        }
        else
        {
            this->LoadPrevPageWithDelayById(CUP_SELECT, 0.0f);
        }
    }

    s32 AddLastLeft(s32 l){
        lastLeftCup += l;
        return 0;


        lefttemp += l;
        if(lefttemp > 1) {
            lastLeftCup += 2; lefttemp = 0;
        }
        if(lefttemp < -1){
            lastLeftCup -= 2; lefttemp = 0;
        }
        return lefttemp;
    }
    s32 GetLastLeft(){
        return lastLeftCup;
     }

    void ExtendCupSelectCupInitSelf(CtrlMenuCupSelectCup * cups)
    {
        u32 CupCount = Cosmos::CupManager::sInstance->GetCupCount();
        cups->curCupID = Cosmos::CupManager::sInstance->lastSelectedCup;
        for(int i = 0; i < 8; i++)
        {
            PushButton * button = &cups->cupButtons[i];
            u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
            id = (id + lastLeftCup + CupCount) % CupCount;
            button->buttonId = id;
            button->SetMsgId(id + BMG_CUPS);
            button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &cups->onCupButtonClickHandler, 0);
            button->SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &cups->onCupButtonSelectHandler);
            button->SetPlayerBitfield(MenuData::sInstance->curScene->Get<Pages::CupSelect>(CUP_SELECT)->GetPlayerBitfield());
            if(id == cups->curCupID){
                button->SelectInitialButton(0);
            }
            CupSelectPlus::ChangeTPL(button, id);
        }
    }
    kmWritePointer(0x808d324c, ExtendCupSelectCupInitSelf);

    void ExtendCourseSelectCourseInitSelf(CtrlMenuCourseSelectCourse* course)
    {
        Cosmos::CupManager * manager = Cosmos::CupManager::sInstance;
        Pages::CupSelect * cup = MenuData::sInstance->curScene->Get<Pages::CupSelect>(CUP_SELECT);
        Pages::CourseSelect * coursePage = MenuData::sInstance->curScene->Get<Pages::CourseSelect>(COURSE_SELECT);

        u32 cupId = manager->lastSelectedCup;

        for(int i = 0; i < 4; i++)
        {
            int slot = manager->currentLayoutArray[cupId * 4 + i];
            course->courseButtons[i].buttonId = slot;
            if(slot >= CT_OFFSET)
                course->courseButtons[i].SetMsgId(slot + BMG_TRACKS);
            else
                course->courseButtons[i].SetMsgId(slot + 9300);

            if(manager->dontUpdateCourseSelectCourse == 0)
            {
                if(i == 0){
                    if((cupId * 4 + 4) < manager->lastSelectedCourse || manager->lastSelectedCourse < (cupId * 4)){
                        coursePage->SelectButton(&course->courseButtons[i]);
                    }
                }

                if(cupId * 4 + i == manager->lastSelectedCourse){
                    coursePage->SelectButton(&course->courseButtons[i]);
                }
            }
        }
        manager->dontUpdateCourseSelectCourse = 0;
    }
    kmWritePointer(0x808d30d8, ExtendCourseSelectCourseInitSelf);

    void UpdateSelection(CupSelectPlus& page, CtrlMenuCupSelectCup * cups, PushButton *button, u32 slotId)
    {
        lastLeftCup = cups->cupButtons[0].buttonId;
        lefttemp = 0;
        for(int i = 0; i < 8; i++)
        {
            PushButton * cbutton = &cups->cupButtons[i];
            if(button == cbutton){
                lastSelectedButton = i;
                lastSelectedCup = cups->cupButtons[i].buttonId;
            }
        }
        RaceData::sInstance->menusScenario.settings.cupId = lastSelectedCup & 0x7;
        page.LoadNextPage(cups, button, slotId);
        Cosmos::CupManager::GetStaticInstance()->lastSelectedCup = lastSelectedCup;

    }
    kmCall(0x807e5da8, UpdateSelection);

    //Disable THP
    kmWrite32(0x808404f8, 0x60000000);

    u32 ReturnCorrectCup(Page * page){
        return Cosmos::CupManager::sInstance->lastSelectedCup;
    }

    kmCall(0x807e6104, ReturnCorrectCup);

    u32 CorrectCourseSelectCup(const Pages::CupSelect& page)
    {
        u32 CupCount = Cosmos::CupManager::sInstance->GetCupCount();
        u32 index = page.clickedCupId;
        s32 id = (index - lastLeftCup + CupCount) % CupCount;
        if(id < 0) id + 8;
        return (id & 1) > 0 ? (4 + (id-1)/2) : id >> 1; 
    }

    asm int asmCorrectCourseSelectCup()
    {
        ASM(
        nofralloc;
        stwu r1, -0x80 (r1);
        stmw r3, 0x8 (r1);

        mflr r12;
        bl CorrectCourseSelectCup;
        mr r0, r3;

        mtlr r12;
        lmw r3, 0x8 (r1);
        addi r1, r1, 0x80;
        mr r26, r0;
        blr;
        );
    }

    kmCall(0x807e45e4, asmCorrectCourseSelectCup);
    
    void ExtendCourseSelectCupInitSelf(CtrlMenuCourseSelectCup* course)
    {
        u32 CupCount = Cosmos::CupManager::sInstance->GetCupCount();
        Cosmos::CupManager * manager = Cosmos::CupManager::GetStaticInstance();
        for(int i = 0; i < 8; i++)
        {
            u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
            id = (id + lastLeftCup + CupCount) % CupCount;

            bool pressed = lastSelectedCup == id;

            CtrlMenuCourseSelectCupSub * button = &course->cupIcons[i];
            button->animator.GetAnimationGroupById(0)->PlayAnimationAtFrame(0,0.0f);
            button->animator.GetAnimationGroupById(1)->PlayAnimationAtFrame(!pressed, 0.0f);
            button->animator.GetAnimationGroupById(2)->PlayAnimationAtFrame(!pressed, 0.0f);
            button->animator.GetAnimationGroupById(3)->PlayAnimationAtFrame(pressed, 0.0f);
            button->selected = pressed;
            button->SetRelativePosition(&course->positionAndscale[1]);
        }
        return;
    }

    //kmWritePointer(0x808d3190, ExtendCourseSelectCupInitSelf);

    void PatchCourseSelectCup()
    {
        u32 CupCount = Cosmos::CupManager::sInstance->GetCupCount();
        Pages::CourseSelect * coursePage = MenuData::sInstance->curScene->Get<Pages::CourseSelect>(COURSE_SELECT);
        Cosmos::CupManager * manager = Cosmos::CupManager::GetStaticInstance();
        for(int i = 0; i < 8; i++)
        {
            u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
            id = (id + lastLeftCup + CupCount) % CupCount;
            char tpl[0x30];
            snprintf(tpl, 0x30, "button/timg/icon_cup_%03d.tpl", id);

            LayoutUIControl * button = &coursePage->ctrlMenuCourseSelectCup.cupIcons[i];

            void * tplPointer = ArchiveRoot::sInstance->GetFile(ARCHIVE_HOLDER_UI, tpl, 0);

            CosmosUI::ChangePaneImage(button, "icon", tplPointer);
            CosmosUI::ChangePaneImage(button, "icon_light_01", tplPointer);
            CosmosUI::ChangePaneImage(button, "icon_light_02", tplPointer);

            button->SetMsgId(BMG_CUPS + id);
        }
        return;
    }
    kmBranch(0x807e4740, PatchCourseSelectCup);

    void FixCourseSelectCup()
    {
        extern u32 p_courseSelectCupId;
        Cosmos::CreateCall((u32)&p_courseSelectCupId, asmCorrectCourseSelectCup);
    }
    static LeCodeLoadHook lclhFixCourseSelectCup(FixCourseSelectCup);

}