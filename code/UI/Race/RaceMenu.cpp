#include "game/System/identifiers.hpp"
#include <UI/Race/RaceMenu.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <game/Race/RaceData.hpp>

namespace CosmosUI {

    TTPausePlus * CreateTTPage() {
        return new TTPausePlus();
    }
    kmCall(0x806235c4, CreateTTPage);
    kmWrite32(0x806235b8, 0x60000000);

    void PatchButtonLoading(PushButton& button, const char * folderName, const char * fileName, u32 index, u32 localPlayerBitfield, u32 r8, bool inaccessible){
        if(index == RaceMenu_ButtonSettings) {
            button.Load(folderName, fileName, "ButtonSettings", localPlayerBitfield, r8, inaccessible);
        }
        else{
            button.Load(folderName, fileName, (const char *) (&Pages::RaceMenu::buttonVariants)[index], localPlayerBitfield, r8, inaccessible);
        }
    }
    //Get the index instead
    kmWrite32(0x80858f4c, 0x7c100378); // mr r16, r0
    kmWrite32(0x80858f60, 0x7e068378); //mr r6, r16
    kmCall(0x80858f78, PatchButtonLoading);

    bool isTimeTrialMode(){
        return RaceData::GetStaticInstance()->racesScenario.GetSettings().gamemode == MODE_TIME_TRIAL;
    }
    static u32 lastTTButton = 0;
    void TTPauseOnButtonClick(Pages::RaceMenu& menu, PushButton* button, u32 val){

        if(isTimeTrialMode())
        {
            menu.nextPage = PAGE_NONE;
            switch(button->buttonId)
            {
                case RaceMenu_ButtonSettings:
                    CosmosUI::NewSettings::SetPreviousPageGlobal(TIME_TRIAL_PAUSE_MENU, TIME_TRIAL_GAMEPLAY);
                    menu.nextPage = (PageId) Cosmos::SETTINGS_MAIN;
                    menu.EndStateAnimate(0.0f,0);
                    break;
                case Pages::RaceMenu::ButtonQuit:
                case Pages::RaceMenu::ButtonChangeCharacter:
                case Pages::RaceMenu::ButtonChangeCourse:
                    menu.nextPage = ARE_YOU_SURE_YOU_WANT_TO_QUIT;
                    menu.EndStateAnimate(0.0f, 0);
                    lastTTButton = button->buttonId;
                    break;
                case Pages::RaceMenu::ButtonConfirmContinue:
                    menu.nextPage = TIME_TRIAL_PAUSE_MENU;
                    menu.EndStateAnimate(0.0f, 1);
                    break;               
                case Pages::RaceMenu::ButtonContinue2:
                    if(RaceInfo::GetStaticInstance()->stage == 0x4) menu.nextPage = SPLITS_AFTER_TT;
                    else menu.nextPage = PAGE_NONE;
                    menu.EndStateAnimate(1, 0.0f);
                    break;
                default:
                    menu.OnButtonClick(button, val);
            }
        }
        else menu.OnButtonClick(button, val);
    }
    kmWritePointer(0x808da844, TTPauseOnButtonClick);

    void ChangeSceneOnConfirm(Pages::RaceMenu* race, MenuId id, u32 animDirection, float a){
        if(isTimeTrialMode){
            switch(lastTTButton){
                case Pages::RaceMenu::ButtonChangeCharacter:
                    id = SINGLE_PLAYER_TT_CHANGE_CHARA; break;
                case Pages::RaceMenu::ButtonChangeCourse:
                    id = SINGLE_PLAYER_TT_CHANGE_COURSE; break;
            }
        }
        race->ChangeMenu(id, animDirection, a);
    }
    kmCall(0x8085a46c, ChangeSceneOnConfirm);
}
