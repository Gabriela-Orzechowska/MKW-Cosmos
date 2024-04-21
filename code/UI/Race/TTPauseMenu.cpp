#include <UI/Race/TTPauseMenu.hpp>
#include <UI/Settings/NewSettingsPage.hpp>

namespace CosmosUI {

    TTPausePlus * CreateTTPage() {
        return new TTPausePlus();
    }
    kmCall(0x806235c4, CreateTTPage);
    kmWrite32(0x806235b8, 0x60000000);

    void PatchButtonLoading(PushButton& button, const char * folderName, const char * fileName, u32 index, u32 localPlayerBitfield, u32 r8, bool inaccessible){
        if(index < RaceMenu_ButtonSettings){     // This is the weirdest part of the code
            button.Load(folderName, fileName, (const char *) (&Pages::RaceMenu::buttonVariants)[index], localPlayerBitfield, r8, inaccessible);
        }
        if(index == RaceMenu_ButtonSettings) {
            button.Load(folderName, fileName, "ButtonSettings", localPlayerBitfield, r8, inaccessible);
        }
    }
    //Get the index instead
    kmWrite32(0x80858f4c, 0x7c100378); // mr r16, r0
    kmWrite32(0x80858f60, 0x7e068378); //mr r6, r16
    kmCall(0x80858f78, PatchButtonLoading);

    void TTPauseOnButtonClick(Pages::RaceMenu& menu, PushButton* button, u32 val){
        if(button->buttonId == RaceMenu_ButtonSettings)
        {
            CosmosUI::NewSettings::SetPreviousPageGlobal(TIME_TRIAL_PAUSE_MENU, TIME_TRIAL_GAMEPLAY);
            menu.nextPage = (PageId) Cosmos::SETTINGS_MAIN;
            menu.EndStateAnimate(0.0f,0);
        }
        else {
            menu.nextPage = PAGE_NONE;
            menu.OnButtonClick(button, val);
        }
    }
    kmWritePointer(0x808da844, TTPauseOnButtonClick);
}