/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Aurora/AuroraSlot.hpp"
#include "Race/RaceData.hpp"
#include "Settings/UserData.hpp"
#include "UI/BMG/BMG.hpp"
#include "UI/Ctrl/Menu/CtrlMenuCup.hpp"
#include "UI/Ctrl/PushButton.hpp"
#include "UI/CupSelect/CourseSelect.hpp"
#include "UI/Page/Menu/CourseSelect.hpp"
#include "UI/Page/Page.hpp"
#include "kamek.hpp"
#include <UI/CupSelect/CupSelect.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuCourse.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <SlotExpansion/SlotExpansion.hpp>

namespace CosmosUI
{
    void ExtendCupSelectCupInitSelf(CtrlMenuCupSelectCup * cups);
    static s32 lefttemp = 0;


    void CupSelectCourseUpdate(CtrlMenuCupSelectCourse& course, int cupId){
        for(int i = 0; i < 4; i++){
           LayoutUIControl& trackName = course.trackNames[i]; 
            trackName.SetMsgId(GetCorrectTrackBMG(Cosmos::CupManager::GetStaticInstance()->GetCurrentTrackList()[cupId * 4 + i]));
        }
    }
    kmBranch(0x807e5ff0, CupSelectCourseUpdate);

    void CupSelectCourseInit(CtrlMenuCupSelectCourse& course){
        int cupId = CupSelectPlus::GetPage()->ctrlMenuCupSelectCup.curCupID;
        for(int i = 0; i < 4; i++){
           LayoutUIControl& trackName = course.trackNames[i]; 
            trackName.SetMsgId(GetCorrectTrackBMG(Cosmos::CupManager::GetStaticInstance()->GetCurrentTrackList()[cupId * 4 + i]));
            trackName.SetPaneVisible("waku_null", false);
            trackName.animator.GetAnimationGroupById(0)->PlayAnimationAtFrame(0, 0.0f);
            trackName.animator.GetAnimationGroupById(1)->PlayAnimationAtFrame(0, 0.0f);
            trackName.animator.GetAnimationGroupById(2)->PlayAnimationAtFrame(1, 0.0f);
            trackName.animator.GetAnimationGroupById(3)->PlayAnimationAtFrame(1, 0.0f);
        }
        course.zIndex = 10.0f;
    }
    kmBranch(0x807e60dc, CupSelectCourseInit);

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
        
        this->ctrlMenuCupSelectCup.curCupID = Cosmos::CupManager::GetStaticInstance()->lastSelectedCup;
        onSwitchPressHandler.subject = this;
        onSwitchPressHandler.ptmf = &CupSelectPlus::OnSwitchPress;
        this->controlsManipulatorManager.SetGlobalHandler(SWITCH_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onSwitchPressHandler, false, false);
    }
    kmWrite32(0x80841244, 0x38800001);

    void CreateCustomCupSelectControl(ControlLoader& control, const char* folder, const char* name, const char* variant, const char** anims){
        control.Load(folder, "CosmosCupSelectNULL", variant, anims);
    }
    kmCall(0x807e56f4, CreateCustomCupSelectControl);
    
    UIControl *  CupSelectPlus::CreateControl(u32 controlId)
    {
        switch(controlId){
            case 2:
                this->AddControl(2, &this->arrows, 0);
                arrows.SetRightArrowHandler((PtmfHolder_2A<Page, void, SheetSelectControl *, u32>*)&this->onRightArrowSelectHandler);
                arrows.SetLeftArrowHandler((PtmfHolder_2A<Page, void, SheetSelectControl *, u32>*)&this->onLeftArrowSelectHandler);
                arrows.Load("button", "CosmosCupSelectRightArrow", "ButtonArrowRight", "CosmosCupSelectLeftArrow", "ButtonArrowLeft", 1, 0, false);
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
        snprintf(tpl, 0x20, "icon_cup_%03x.tpl", id);
        CosmosUI::ChangePaneImage(ctrl, "icon", tpl);
        CosmosUI::ChangePaneImage(ctrl, "icon_light_01", tpl);
        CosmosUI::ChangePaneImage(ctrl, "icon_light_02", tpl);
    }

    void CupSelectPlus::ScrollCups(s32 direction)
    {
        u32 CupCount = Cosmos::CupManager::GetStaticInstance()->GetCupCount();
        CtrlMenuCupSelectCup *cupCtrl = &this->ctrlMenuCupSelectCup;
        u32 lastCup = cupCtrl->curCupID;
        cupCtrl->curCupID = (cupCtrl->curCupID + (direction * 2) + CupCount) % CupCount;
        this->ctrlMenuCupSelectCourse.UpdateTrackList(cupCtrl->curCupID);
        u32 cupOffset = Cosmos::CupManager::GetStaticInstance()->GetIconOffset();
        for(int i = 0; i < 8; i++)
        {
            PushButton * button = &this->ctrlMenuCupSelectCup.cupButtons[i];
            u32 id = button->buttonId;
            
            if(id == lastCup) button->Select(0);

            id = (id + (direction * 2) + CupCount) % CupCount;
            button->SetMsgId(id + BMG_CUPS + cupOffset);
            button->buttonId = id;
            this->ChangeTPL(button, id + cupOffset);
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
        u32 sorting = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_SORTING);
        u32 trackList = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetTrackList();

        Cosmos::CupManager::GetStaticInstance()->SetTrackLayout(sorting, trackList);

        u32 cupCupId = Cosmos::CupManager::GetStaticInstance()->lastSelectedCup;
        this->ctrlMenuCupSelectCup.curCupID = cupCupId;
        Pages::CupSelect::OnActivate();
        lefttemp = 0;

        u32 bmg = 0x2810 + sorting;
        if(RaceData::GetStaticInstance()->menusScenario.GetSettings().gamemode == MODE_GRAND_PRIX)
            bmg = 0;
        this->bottomText->SetMsgId(bmg);

        this->ctrlMenuCupSelectCourse.UpdateTrackList(this->ctrlMenuCupSelectCup.curCupID);
        //ExtendCupSelectCupInitSelf(&this->ctrlMenuCupSelectCup);
    }

    void CupSelectPlus::OnSwitchPress(u32 slotId)
    {        
        if(RaceData::GetStaticInstance()->menusScenario.GetSettings().gamemode == MODE_GRAND_PRIX) return;

        Cosmos::Data::SettingsHolder* holder = Cosmos::Data::SettingsHolder::GetStaticInstance();
        COSMOS_ASSERT_NOT_NULL(holder);
        u32 sorting = holder->GetSettingValue(Cosmos::Data::COSMOS_SETTING_SORTING);
        sorting ^= 1;
        holder->SetSettingValue(sorting, Cosmos::Data::COSMOS_SETTING_SORTING);

        Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance();
        manager->SetTrackLayout(sorting);
        manager->lastSelectedCup = this->ctrlMenuCupSelectCup.curCupID;
        for(int i = 0; i < 8; i++){
            if(this->ctrlMenuCupSelectCup.curCupID == this->ctrlMenuCupSelectCup.cupButtons[i].buttonId){
                manager->lastSelectedButton = i;
                break;
            }
        }
        this->bottomText->SetMsgId(0x2810 + sorting);

        if(sorting == Cosmos::Data::SORTING_ALPHABETICAL)
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
        u32 CupCount = Cosmos::CupManager::GetStaticInstance()->GetCupCount();
        cups->curCupID = Cosmos::CupManager::GetStaticInstance()->lastSelectedCup;
        u32 cupOffset = Cosmos::CupManager::GetStaticInstance()->GetIconOffset();
        for(int i = 0; i < 8; i++)
        {
            PushButton * button = &cups->cupButtons[i];
            u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
            id = (id + lastLeftCup + CupCount) % CupCount;
            button->buttonId = id;
            button->SetMsgId(id + BMG_CUPS + cupOffset);
            button->SetOnClickHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &cups->onCupButtonClickHandler, 0);
            button->SetOnSelectHandler((PtmfHolder_2A<Page, void, PushButton *, u32>*) &cups->onCupButtonSelectHandler);
            button->SetPlayerBitfield(MenuData::GetStaticInstance()->curScene->Get<Pages::CupSelect>(CUP_SELECT)->GetPlayerBitfield());
            if(id == cups->curCupID){
                button->SelectInitialButton(0);
            }
            CupSelectPlus::ChangeTPL(button, id + cupOffset);
        }
    }
    kmWritePointer(0x808d324c, ExtendCupSelectCupInitSelf);

    CtrlMenuCourseSelectCupSub* GetActiveCupIcon(CtrlMenuCourseSelectCup& cup){
        for(int i = 0; i < 8; i++){
            if(cup.cupIcons[i].selected) return &cup.cupIcons[i];
        }
        return nullptr;
    }

    void LumpCheck(Pages::CourseSelect& page, u32 slot, u32 i){
        CtrlMenuCourseSelectCourse* course = &page.ctrlMenuCourseSelectCourse;
        if(course->courseButtons[i].IsSelected()){
            static bool isLump = false;
            if(!isLump && slot == Aurora::Special::SLOT_LUMPYS){
                CtrlMenuCourseSelectCupSub* curButton = GetActiveCupIcon(page.ctrlMenuCourseSelectCup);
                curButton->SetMsgId(0x70000);
                void * tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, "button/timg/icon_cup_lump.tpl", 0);
                CosmosUI::ChangePaneImage(curButton, "icon", tplPointer);
                CosmosUI::ChangePaneImage(curButton, "icon_light_01", tplPointer);
                CosmosUI::ChangePaneImage(curButton, "icon_light_02", tplPointer);
            }
            else if (isLump){
                CtrlMenuCourseSelectCupSub* curButton = GetActiveCupIcon(page.ctrlMenuCourseSelectCup);
                curButton->SetMsgId(BMG_CUPS + curButton->padding_id);
                char tpl[0x30];
                snprintf(tpl, 0x30, "button/timg/icon_cup_%03x.tpl", curButton->padding_id);
                void * tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, tpl, 0);
                CosmosUI::ChangePaneImage(curButton, "icon", tplPointer);
                CosmosUI::ChangePaneImage(curButton, "icon_light_01", tplPointer);
                CosmosUI::ChangePaneImage(curButton, "icon_light_02", tplPointer);
                isLump = false;
            }
        }
    };

    void ExtendCourseSelectCourseInitSelf(CtrlMenuCourseSelectCourse* course)
    {
        Cosmos::CupManager * manager = Cosmos::CupManager::GetStaticInstance();
        Pages::CupSelect * cup = MenuData::GetStaticInstance()->curScene->Get<Pages::CupSelect>(CUP_SELECT);
        Pages::CourseSelect * coursePage = MenuData::GetStaticInstance()->curScene->Get<Pages::CourseSelect>(COURSE_SELECT);
        if(coursePage->currentState == STATE_UNLOADED) {
            CosmosUI::VariantSelectPlus* variant = VariantSelectPlus::GetPage();
            COSMOS_ASSERT_NOT_NULL(variant);
            variant->SelectButton(&variant->ctrlMenuCourseSelectCourse.courseButtons[0]);
            return;
        }

        u32 cupId = manager->lastSelectedCup;
        bool selected = false;
        for(int i = 0; i < 4; i++)
        {
            int slot = manager->GetCurrentTrackList()[cupId * 4 + i];
            course->courseButtons[i].buttonId = slot;
            if(Cosmos::isCTSlot(slot))
                course->courseButtons[i].SetMsgId(slot + BMG_TRACKS);
            else if(Cosmos::isGroupSlot(slot))
                course->courseButtons[i].SetMsgId(slot + BMG_GROUPS);
            else
                course->courseButtons[i].SetMsgId(slot + 9300);

            if(manager->dontUpdateCourseSelectCourse == 0)
            {
                if(slot == manager->lastSelectedCourse || slot == manager->lastSelectedGroup){
                    coursePage->SelectButton(&course->courseButtons[i]);
                    selected = true;
                    LumpCheck(*coursePage, slot, i);
                }
            }
            else {
                selected = true; 
                LumpCheck(*coursePage, slot, i);
            }
        }
        if(!selected){
            coursePage->SelectButton(&course->courseButtons[0]);
        }
        manager->dontUpdateCourseSelectCourse = 0;
    }
    kmWritePointer(0x808d30d8, ExtendCourseSelectCourseInitSelf);

    void CupSelectPlus::UpdateSelection(CtrlMenuCupSelectCup * cups, PushButton *button, u32 slotId)
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
        RaceData::GetStaticInstance()->menusScenario.GetSettings().cupId = lastSelectedCup & 0x7;
        this->LoadNextPage(cups, button, slotId);
        Cosmos::CupManager::GetStaticInstance()->lastSelectedCup = lastSelectedCup;

    }

    void UpdateSelection(CupSelectPlus& page, CtrlMenuCupSelectCup* cups, PushButton* button, u32 slotId){
        page.UpdateSelection(cups,button,slotId);
    }

    kmCall(0x807e5da8, UpdateSelection);

    void UpdateCupPageBottomText(){
        CupSelectPlus* page = CupSelectPlus::GetPage();
        u32 sorting = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_SORTING); 

        u32 bmg = 0x2810 + sorting;
        if(RaceData::GetStaticInstance()->menusScenario.GetSettings().gamemode == MODE_GRAND_PRIX)
            bmg = 0;

        page->bottomText->SetMsgId(bmg);
    }
    kmBranch(0x807e5d64, UpdateCupPageBottomText);

    //Disable THP
    kmWrite32(0x808404f8, 0x60000000);

    u32 ReturnCorrectCup(Page * page){
        return Cosmos::CupManager::GetStaticInstance()->lastSelectedCup;
    }

    kmCall(0x807e6104, ReturnCorrectCup);

    u32 CorrectCourseSelectCup(const Pages::CupSelect& page)
    {
        u32 CupCount = Cosmos::CupManager::GetStaticInstance()->GetCupCount();
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
        u32 CupCount = Cosmos::CupManager::GetStaticInstance()->GetCupCount();
        Cosmos::CupManager * manager = Cosmos::CupManager::GetStaticInstance();
        for(int i = 0; i < 8; i++)
        {
            CtrlMenuCourseSelectCupSub * button = &course->cupIcons[i];
            u32 id = button->padding_id;

            bool pressed = lastSelectedCup == id;

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
        u32 CupCount = Cosmos::CupManager::GetStaticInstance()->GetCupCount();
        Pages::CourseSelect * coursePage = MenuData::GetStaticInstance()->curScene->Get<Pages::CourseSelect>(COURSE_SELECT);
        Cosmos::CupManager * manager = Cosmos::CupManager::GetStaticInstance();
        u32 cupOffset = manager->GetIconOffset();
        for(int i = 0; i < 8; i++)
        {
            u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
            id = (id + lastLeftCup + CupCount) % CupCount;
            char tpl[0x30];
            snprintf(tpl, 0x30, "button/timg/icon_cup_%03x.tpl", id + cupOffset);

            CtrlMenuCourseSelectCupSub * button = &coursePage->ctrlMenuCourseSelectCup.cupIcons[i];
            button->padding_id = id;

            void * tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, tpl, 0);

            CosmosUI::ChangePaneImage((LayoutUIControl*) button, "icon", tplPointer);
            CosmosUI::ChangePaneImage((LayoutUIControl*) button, "icon_light_01", tplPointer);
            CosmosUI::ChangePaneImage((LayoutUIControl*) button, "icon_light_02", tplPointer);

            button->SetMsgId(BMG_CUPS + id + cupOffset);
        }
        return;
    }
    kmBranch(0x807e4740, PatchCourseSelectCup);
}
