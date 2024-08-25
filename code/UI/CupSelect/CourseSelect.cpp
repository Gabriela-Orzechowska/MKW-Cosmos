#include "Sound/SoundId.hpp"
#include "System/Identifiers.hpp"
#include "UI/BMG/BMG.hpp"
#include "main.hpp"
#include <game/UI/Ctrl/Menu/CtrlMenuCourse.hpp>
#include <UI/CupSelect/CourseSelect.hpp>


namespace CosmosUI
{
    extern s32 lastLeftCup;

    CourseSelectPlus* CreateCoursePage()
    {
        CourseSelectPlus * course = new(CourseSelectPlus);
        return course;
    };

    kmWrite32(0x80623db0, 0x60000000);
    kmCall(0x80623dbc, CreateCoursePage);

    CourseSelectPlus::CourseSelectPlus(){
        internControlCount += 1;
        onRightClick.subject = this;
        onRightClick.ptmf = &CourseSelectPlus::OnRightClick;
        onLeftClick.subject = this;
        onLeftClick.ptmf = &CourseSelectPlus::OnLeftClick;

        this->controlsManipulatorManager.SetGlobalHandler(RIGHT_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onRightClick, false, false);
        this->controlsManipulatorManager.SetGlobalHandler(LEFT_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onLeftClick, false, false);
        skipNextAnim = false;
    }

    UIControl * CourseSelectPlus::CreateControl(u32 controlId)
    {
        switch(controlId){
            case 2:
                this->AddControl(2, &this->arrows, 0);
                arrows.Load("button", "CosmosCourseSelectRightArrow", "ButtonArrowRight", "CosmosCourseSelectLeftArrow", "ButtonArrowLeft", 1, 0, true);
                this->controlCount++;
                CosmosLog("Arrow Pointer: %p\n", &this->arrows);
                return &this->arrows;
            default:
                return CourseSelect::CreateControl(controlId);
        }
    }


    void CourseSelectPlus::OnButtonClick(s32 hudSlotId, PushButton& button){
        
    };

    void CourseSelectPlus::SlideCup(s32 offset)
    {
        CtrlMenuCourseSelectCup * courseCup = &this->ctrlMenuCourseSelectCup;
        Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance();
        u32 cupCount = manager->GetCupCount();
        Pages::CupSelect * cup = MenuData::GetStaticInstance()->curScene->Get<Pages::CupSelect>(CUP_SELECT);
        manager->lastSelectedCup = (manager->lastSelectedCup + offset + cupCount) % cupCount;
        lastSelectedCup = manager->lastSelectedCup;
        cup->clickedCupId = lastSelectedCup;

        //this->LoadPrevPageWithDelayById(COURSE_SELECT, 0.0f);
        CtrlMenuCupSelectCup* cups = &cup->ctrlMenuCupSelectCup;

        manager->dontUpdateCourseSelectCourse = 1;
        int off = AddLastLeft(offset);
        PatchCourseSelectCup();     

        if(offset > 0){
            this->arrows.rightArrow.animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0,0.0f);
        }
        else{
            this->arrows.leftArrow.animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0,0.0f);
        }
        this->arrows.HandleLeftArrowSelect(0); //Works for sound
        for(int i = 0; i < 8; i++)
        {
            CtrlMenuCourseSelectCupSub * button = &courseCup->cupIcons[i];
            if(button->selected)
            {
                u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
                id = (id + GetLastLeft() + off + cupCount) % cupCount;
                char tpl[0x30];
                snprintf(tpl, 0x30, "button/timg/icon_cup_%03x.tpl", id);

                const void* tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, tpl, 0);

                CosmosUI::ChangePaneImage(button, "icon", tplPointer);
                CosmosUI::ChangePaneImage(button, "icon_light_01", tplPointer);
                CosmosUI::ChangePaneImage(button, "icon_light_02", tplPointer);

                button->SetMsgId(BMG_CUPS + id);
            }
        } 

        ExtendCourseSelectCourseInitSelf(&this->ctrlMenuCourseSelectCourse);
    }

    void CourseSelectPlus::BeforeEntranceAnimations(){
        Pages::CourseSelect::BeforeEntranceAnimations();
        if(!skipNextAnim) return;
        for(int i = 0; i < 8; i++){
            CtrlMenuCourseSelectCupSub& icon = this->ctrlMenuCourseSelectCup.cupIcons[i];
            if(icon.selected) {
                icon.frame = 300.0f;
            }
        }
        skipNextAnim = false;
        this->PlaySound(Sound::SE_UI_PAGE_PREV, -1);
    }

    void OnCourseButtonClickOverride(CtrlMenuCourseSelectCourse& course, PushButton& button, s32 hudSlotId){
        CourseSelectPlus* page = CourseSelectPlus::GetPage();
        VariantSelectPlus* variantSelect = VariantSelectPlus::GetPage();
        COSMOS_ASSERT_NOT_NULL(variantSelect);

        u32 trackIndex = button.buttonId;

        if(page->isFocused()){
            if(Cosmos::isGroupSlot(trackIndex)){
                variantSelect->SetupPage(trackIndex);
                Cosmos::CupManager::GetStaticInstance()->lastSelectedGroup = (trackIndex);
                Cosmos::CupManager::GetStaticInstance()->lastSelectedCourse = -1U;
                page->LoadNextPageWithDelayById((PageId)Cosmos::VARIANT_SELECT,0.0f);
            }
            else {
                Cosmos::CupManager::GetStaticInstance()->lastSelectedGroup = -1;
                page->LoadNextPage(&course, &button, hudSlotId);
            }
        }
        else if(variantSelect->isFocused()){
            variantSelect->LoadNextPage(&course, &button, hudSlotId);
        }
    };

    kmBranch(0x807e5434, OnCourseButtonClickOverride);

    VariantSelectPlus::VariantSelectPlus() {
        internControlCount++;
        currentSubPage = 0;
        subPageCount = 1;

        onRightClick.subject = this;
        onRightClick.ptmf = &VariantSelectPlus::OnRightClick;
        onLeftClick.subject = this;
        onLeftClick.ptmf = &VariantSelectPlus::OnLeftClick;

        this->controlsManipulatorManager.SetGlobalHandler(RIGHT_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onRightClick, false, false);
        this->controlsManipulatorManager.SetGlobalHandler(LEFT_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onLeftClick, false, false);
        this->onBackPressHandler.ptmf = static_cast<void (Page::*)(u32)> (&VariantSelectPlus::OnBackPressNew);
    };

    UIControl* VariantSelectPlus::CreateControl(u32 controlId){
        //TODO Update;
        switch(controlId){
            case 2:
                this->AddControl(2, &this->arrows, 0);
                arrows.Load("button", "CosmosVariantSelectRightArrow", "ButtonArrowRight", "CosmosVariantSelectLeftArrow", "ButtonArrowLeft", 1, 0, true);
                this->controlCount++;
                return &this->arrows;
            default:
                return Pages::CourseSelect::CreateControl(controlId);
        }
    };

    void VariantSelectPlus::OnBackPressNew(u32 hudSlotId){
        CourseSelectPlus::GetPage()->SkipAnimation();
        this->LoadPrevPageWithDelayById(COURSE_SELECT, 0.0f);
    }

    void VariantSelectPlus::SetupPage(u32 slot){
        this->SelectButton(&this->ctrlMenuCourseSelectCourse.courseButtons[0]);
        this->currentGroup = slot;

        Cosmos::CupManager* manager = Cosmos::CupManager::GetStaticInstance();
        variant = manager->GetVariantStruct(slot);
        currentSlot = slot;
        COSMOS_ASSERT_NOT_NULL(variant);
        
        this->subPageCount = (variant->count + 3) / 3;
        this->currentSubPage = 0;

        this->SetupButtons();
    }

    void VariantSelectPlus::BeforeEntranceAnimations(){
        Pages::Menu::BeforeEntranceAnimations();
        for(int i = 0; i < 8; i++){
            CtrlMenuCourseSelectCupSub& icon = this->ctrlMenuCourseSelectCup.cupIcons[i];
            if(icon.selected) {
                icon.frame = 300.0f;
                icon.isHidden = false;
                icon.SetMsgId(BMG_GROUPS + this->currentGroup);

                const void* tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, "button/timg/st_battle_icon_flag.tpl", 0);
                CosmosUI::ChangePaneImage(&icon, "icon", tplPointer);
                CosmosUI::ChangePaneImage(&icon, "icon_light_01", tplPointer);
                CosmosUI::ChangePaneImage(&icon, "icon_light_02", tplPointer);
            }
            else
                icon.isHidden = true;
        }
        this->SetupButtons();
    };

    void VariantSelectPlus::SetupButtons(){
        bool hasRandom = false;
        u32 totalButtonCount = (variant->count) + hasRandom;
        u32 buttonCount = totalButtonCount - (4 * this->currentSubPage);
        if(buttonCount > 4) buttonCount = 4;
        if(this->subPageCount > 1){
            this->arrows.rightArrow.isHidden = false;
            this->arrows.leftArrow.isHidden = false;
        }
        else {
            this->arrows.rightArrow.isHidden = true;
            this->arrows.leftArrow.isHidden = true;
        }
        //TODO Allow hiding Random button
        CtrlMenuCourseSelectCourse& courseSelect = this->ctrlMenuCourseSelectCourse;
        int offset = this->currentSubPage * 4;
        for(int i = 0; i < 4; i++){
            PushButton& button = courseSelect.courseButtons[i];
            if(i == 0){
                this->SelectButton(&button);
            }
            if(i < buttonCount) {
                button.isHidden = false;
                button.SetPlayerBitfield(0x1);
                if(hasRandom && ((offset + i) == 0)){
                    button.SetMsgId(0xd72);
                    button.buttonId = this->currentGroup;
                }
                else {
                    button.SetMsgId(BMG_VARIANTS + ((currentSlot & ~0x3000)) * 0x10 + (i-(int)hasRandom));
                    button.buttonId = this->variant->slot[offset + i - (int)hasRandom];
                }
            }
            else {
                button.HandleDeselect(0,0);
                button.isHidden = true;
                button.SetPlayerBitfield(0);
            }
        }
    }

    void VariantSelectPlus::OnLeftClick(u32 hudSlotId){
        if(this->currentSubPage != 0) {
            this->currentSubPage--;
            this->arrows.leftArrow.animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0,0.0f);
            this->arrows.HandleLeftArrowSelect(0); //Works for sound
            this->SetupButtons();
        }
    }
    void VariantSelectPlus::OnRightClick(u32 hudSlotId){
        if((this->currentSubPage + 1) != this->subPageCount) {
            this->currentSubPage++;
            this->arrows.rightArrow.animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0,0.0f);
            this->arrows.HandleLeftArrowSelect(0); //Works for sound
            this->SetupButtons();
        }
    }

} // namespace CosmosUI
