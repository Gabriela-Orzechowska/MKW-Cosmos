#include "UI/BMG/BMG.hpp"
#include "UI/CupSelect/CourseSelect.hpp"
#include "UI/Page/Menu/CourseSelect.hpp"
#include "hooks.hpp"
#include <kamek.hpp>
#include <Aurora/AuroraSlot.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuCourse.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceTime.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceLap.hpp>
#include <UI/Language/LanguageManager.hpp>

namespace Aurora {
    namespace Special {
    
        void LoadTimeControl(CtrlRaceTime& time, const char* variant, u8 hudSlotId){
            time.Load(variant, hudSlotId);
            u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
            if(trackId != SLOT_LUMPYS) return;

            if(time.layout.GetPaneByName("set_p") == nullptr) return;

            int curLang = Cosmos::LanguageManager::GetStaticInstance()->GetActualLanguage();

            void* tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, "game_image/timg/tt_ml_lump.tpl", 0);
            if(curLang == Cosmos::LanguageManager::POLISH || curLang == Cosmos::LanguageManager::CZECH || curLang == Cosmos::LanguageManager::HUNGARIAN) {
                time.layout.GetPaneByName("set_p")->scale.z *= 0.5f;
            }

            CosmosUI::ChangePaneImage(&time, "set_p", tplPointer);
        }
        kmCall(0x80858188, LoadTimeControl);

        void LoadLapControl(CtrlRaceLap& lap, const char* variant, u8 hudSlotId){
            lap.Load(variant, hudSlotId);
            u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
            if(trackId != SLOT_LUMPYS) return;

            if(lap.layout.GetPaneByName("lap_text") == nullptr) return;

            int curLang = Cosmos::LanguageManager::GetStaticInstance()->GetActualLanguage();

            void* tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, "game_image/timg/tt_ml_lump.tpl", 0);
            if(curLang == Cosmos::LanguageManager::POLISH || curLang == Cosmos::LanguageManager::CZECH || curLang == Cosmos::LanguageManager::HUNGARIAN)
                lap.layout.GetPaneByName("lap_text")->scale.z *= 0.5f;

            CosmosUI::ChangePaneImage(&lap, "lap_text", tplPointer);
        }
        kmCall(0x80857e60, LoadLapControl);
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

            CtrlMenuCourseSelectCupSub* curButton = GetActiveCupIcon(courseSelect->ctrlMenuCourseSelectCup);
            if(curButton == nullptr) return;
            if(button.buttonId == SLOT_LUMPYS){
                if(!isLump){
                    curButton->SetMsgId(0x70000);
                    void * tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, "button/timg/icon_cup_lump.tpl", 0);
                    CosmosUI::ChangePaneImage(curButton, "icon", tplPointer);
                    CosmosUI::ChangePaneImage(curButton, "icon_light_01", tplPointer);
                    CosmosUI::ChangePaneImage(curButton, "icon_light_02", tplPointer);
                    isLump = true; 
                }
            }
            else if(isLump){
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
