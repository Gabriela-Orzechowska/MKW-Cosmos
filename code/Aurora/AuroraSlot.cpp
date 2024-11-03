#include "UI/BMG/BMG.hpp"
#include "UI/CupSelect/CourseSelect.hpp"
#include "UI/Page/Menu/CourseSelect.hpp"
#include <kamek.hpp>
#include <Aurora/AuroraSlot.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuCourse.hpp>

namespace Aurora {
    namespace Special {
    
        CtrlMenuCourseSelectCupSub* GetActiveCupIcon(CtrlMenuCourseSelectCup& cup){
            for(int i = 0; i < 8; i++){
                if(cup.cupIcons[i].selected) return &cup.cupIcons[i];
            }
            return nullptr;
        }

        void OnCourseButtonSelect(CtrlMenuCourseSelectCourse& course, PushButton& button, u32 hudSlotId){
            static bool isLump = false;
            CosmosUI::CourseSelectPlus* courseSelect = CosmosUI::CourseSelectPlus::GetPage();
            courseSelect->UpdateBottomText(&course, &button, hudSlotId);

            if(button.buttonId == SLOT_LUMPYS){
                if(!isLump){
                    CtrlMenuCourseSelectCupSub* curButton = GetActiveCupIcon(courseSelect->ctrlMenuCourseSelectCup);
                    curButton->SetMsgId(0x70000);
                    void * tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, "button/timg/icon_cup_lump.tpl", 0);
                    CosmosUI::ChangePaneImage(curButton, "icon", tplPointer);
                    CosmosUI::ChangePaneImage(curButton, "icon_light_01", tplPointer);
                    CosmosUI::ChangePaneImage(curButton, "icon_light_02", tplPointer);
                    isLump = true; 
                }
            }
            else if(isLump){
                CtrlMenuCourseSelectCupSub* curButton = GetActiveCupIcon(courseSelect->ctrlMenuCourseSelectCup);
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

        kmBranch(0x807e548c, OnCourseButtonSelect);
    }
}
