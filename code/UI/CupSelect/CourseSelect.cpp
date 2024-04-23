#include <UI/CupSelect/CourseSelect.hpp>

namespace CosmosUI
{
    extern s32 lastLeftCup;

    CourseSelectPlus * CreateCoursePage()
    {
        CourseSelectPlus * course = new(CourseSelectPlus);
        return course;
    };

    kmWrite32(0x80623db0, 0x60000000);
    kmCall(0x80623dbc, CreateCoursePage);
    void SlideCup(CtrlMenuCourseSelectCourse * course, CtrlMenuCourseSelectCup* courseCup, s32 offset);

    CourseSelectPlus::CourseSelectPlus(){
        internControlCount += 1;
        onRightClick.subject = this;
        onRightClick.ptmf = &CourseSelectPlus::OnRightClick;
        onLeftClick.subject = this;
        onLeftClick.ptmf = &CourseSelectPlus::OnLeftClick;

        this->controlsManipulatorManager.SetGlobalHandler(RIGHT_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onRightClick, false, false);
        this->controlsManipulatorManager.SetGlobalHandler(LEFT_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onLeftClick, false, false);
    }

    UIControl * CourseSelectPlus::CreateControl(u32 controlId)
    {
        switch(controlId){
            case 2:
                this->AddControl(2, &this->arrows, 0);
                arrows.Load("button", "DXCupSelectRightArrow", "ButtonArrowRight", "DXCupSelectLeftArrow", "ButtonArrowLeft", 1, 0, true);
                this->controlCount++;
                return &this->arrows;
            default:
                return CourseSelect::CreateControl(controlId);
        }
    }

    static s32 slide = 0;

    void SlideCup(CourseSelectPlus * coursePage, s32 offset)
    {
        CtrlMenuCourseSelectCourse * course = &coursePage->CtrlMenuCourseSelectCourse;
        CtrlMenuCourseSelectCup * courseCup = &coursePage->ctrlMenuCourseSelectCup;
        Cosmos::CupManager * manager = Cosmos::CupManager::GetStaticInstance();
        u32 cupCount = manager->GetCupCount();
        Pages::CupSelect * cup = MenuData::GetStaticInstance()->curScene->Get<Pages::CupSelect>(CUP_SELECT);
        manager->lastSelectedCup = (manager->lastSelectedCup + offset + cupCount) % cupCount;
        lastSelectedCup = manager->lastSelectedCup;
        cup->clickedCupId = lastSelectedCup;

        //this->LoadPrevPageWithDelayById(COURSE_SELECT, 0.0f);
            CtrlMenuCupSelectCup * cups = &cup->ctrlMenuCupSelectCup;

        manager->dontUpdateCourseSelectCourse = 1;
        int off = AddLastLeft(offset);
        PatchCourseSelectCup();     

        if(offset > 0){
            coursePage->arrows.rightArrow.animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0,0.0f);
        }
        else{
            coursePage->arrows.leftArrow.animator.GetAnimationGroupById(ANM_SELECT)->PlayAnimationAtFrame(0,0.0f);
        }
        coursePage->arrows.HandleLeftArrowSelect(0); //Works for sound
        for(int i = 0; i < 8; i++)
        {
            CtrlMenuCourseSelectCupSub * button = &courseCup->cupIcons[i];
            if(button->selected)
            {
                u32 id = i < 4 ? i * 2 : ((i-4) * 2) + 1;
                id = (id + GetLastLeft() + off + cupCount) % cupCount;
                char tpl[0x30];
                snprintf(tpl, 0x30, "button/timg/icon_cup_%03d.tpl", id);

                void * tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, tpl, 0);

                CosmosUI::ChangePaneImage(button, "icon", tplPointer);
                CosmosUI::ChangePaneImage(button, "icon_light_01", tplPointer);
                CosmosUI::ChangePaneImage(button, "icon_light_02", tplPointer);

                button->SetMsgId(BMG_CUPS + id);
            }
        } 

        ExtendCourseSelectCourseInitSelf(course);
    }

    void CourseSelectPlus::OnRightClick(u32 slotId){
        SlideCup(this, 1);
    }

    void CourseSelectPlus::OnLeftClick(u32 slotId){
        SlideCup(this, -1);
    }

} // namespace CosmosUI
