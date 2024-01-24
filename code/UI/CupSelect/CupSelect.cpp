#include <UI/CupSelect/CupSelect.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuCourse.hpp>
#include <LeCode/LeCodeManager.hpp>

namespace DXUI
{
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
        DXUI::ChangePaneImage(ctrl, "icon", tpl);
        DXUI::ChangePaneImage(ctrl, "icon_light_01", tpl);
        DXUI::ChangePaneImage(ctrl, "icon_light_02", tpl);
    }

    void CupSelectPlus::ScrollCups(s32 direction)
    {
        CtrlMenuCupSelectCup *cupCtrl = &this->ctrlMenuCupSelectCup;
        cupCtrl->curCupID = (cupCtrl->curCupID + (direction * 2) + CUP_COUNT) % CUP_COUNT;
        for(int i = 0; i < 8; i++)
        {
            PushButton * button = &this->ctrlMenuCupSelectCup.cupButtons[i];
            u32 id = button->buttonId;
            id = (id + (direction * 2) + CUP_COUNT) % CUP_COUNT;
            button->SetMsgId(id + BMG_CUPS);
            button->buttonId = id;
            this->ChangeTPL(button, id);
        }
        this->ctrlMenuCupSelectCourse.UpdateTrackList(cupCtrl->curCupID);
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
        Pages::CupSelect::OnActivate();
        this->bottomText->SetMsgId(0x2810 + (u32)currentLayout);
    }

    void CupSelectPlus::OnSwitchPress(u32 slotId)
    {        
        currentLayout = static_cast<CUP_LAYOUT>(((u32)currentLayout) ^ 0x1);

        DVDFileInfo fileHandle;
        char * filepath = "/bin/cup2.bin";

        char buffer[0x20] __attribute__ ((aligned(0x20)));

        if(DVDOpen(filepath, &fileHandle))
        {
            u32 offset = ((u32)currentLayout) * CUPFILE_SORT_OFFSET;

            if(DVDReadPrio(&fileHandle, buffer, 0x10, offset, 0x2))
            {
                LeCode::cup2_header * header = (LeCode::cup2_header * ) &buffer;

                u32 num_bytes = 0x10 + 0x10 * (header->course_cups);
                char actualBuffer[0x1000] __attribute__ ((aligned(0x20)));

                char * address = (char *) LeCode::LeCodeManager::GetStaticInstance()->GetCupParamAddress();

                DVDReadPrio(&fileHandle, actualBuffer, num_bytes, offset, 0x2);
                DVDClose(&fileHandle);
                memcpy(address, actualBuffer, num_bytes);
            }
            else {
                return;
            }
        }
        else return;

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

    void ExtendCupSelectCupInitSelf(CtrlMenuCupSelectCup * cups)
    {
        cups->curCupID = lastSelectedCup;
        for(int i = 0; i < 8; i++)
        {
            PushButton * button = &cups->cupButtons[i];
            u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
            id = (id + lastLeftCup + CUP_COUNT) % CUP_COUNT;
            button->buttonId = id;
            button->SetMsgId(id + BMG_CUPS);
            button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &cups->onCupButtonClickHandler, 0);
            button->SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &cups->onCupButtonSelectHandler);
            button->SetPlayerBitfield(MenuData::sInstance->curScene->Get<Pages::CupSelect>(CUP_SELECT)->GetPlayerBitfield());
            CupSelectPlus::ChangeTPL(button, id);
        }
        cups->cupButtons[lastSelectedButton].SelectInitialButton(0);
    }
    kmWritePointer(0x808d324c, ExtendCupSelectCupInitSelf);

    void UpdateSelection(CupSelectPlus * page, CtrlMenuCupSelectCup * cups, PushButton *button, u32 slotId)
    {
        lastLeftCup = cups->cupButtons[0].buttonId;

        for(int i = 0; i < 8; i++)
        {
            PushButton * cbutton = &cups->cupButtons[i];
            if(button == cbutton){
                lastSelectedButton = i;
            }
        }
        page->LoadNextPage(cups, button, slotId);
        RaceData::sInstance->menusScenario.settings.cupId = lastSelectedCup % 8;
    }
    kmCall(0x807e5da8, UpdateSelection);

    //Disable THP
    kmWrite32(0x808404f8, 0x60000000);

    u32 CorrectCourseSelectCup(Pages::CupSelect * page)
    {
        s32 offset = page->ctrlMenuCupSelectCup.cupButtons[0].buttonId;
        s32 id = page->clickedCupId - offset;
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

    void FixCourseSelectCup()
    {
        extern u32 p_courseSelectCupId;
        DX::CreateCall((u32)&p_courseSelectCupId, asmCorrectCourseSelectCup);
    }
    static LeCodeLoadHook lclhFixCourseSelectCup(FixCourseSelectCup);

}