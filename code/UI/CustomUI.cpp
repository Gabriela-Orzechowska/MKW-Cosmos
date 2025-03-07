/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2025 Gabriela Orzechowska
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


#include "Aurora/AuroraLicense.hpp"
#include "Debug/Debug.hpp"
#include "Debug/IOSDolphin.hpp"
#include "System/Identifiers.hpp"
#include "UI/CupSelect/CourseSelect.hpp"
#include "game/System/identifiers.hpp"
#include "hooks.hpp"
#include <kamek.hpp>
#include <System/System.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <UI/Settings/MessageWarning.hpp>
#include <game/UI/Page/RaceMenu/TTPause.hpp>
#include <game/UI/Page/Menu/TopMenuOverlay.hpp>
#include <System/Security.hpp>
#include <Aurora/AuroraSecurity.hpp>
#include <Aurora/AuroraAPI.hpp>
#include <Aurora/AuroraLicense.hpp>
#include <UI/Leaderboards/LeaderboardMain.hpp>

void* CreatePage(u32 pageId)
{
    switch(pageId){
        case Cosmos::SETTINGS_MAIN:
            return new (CosmosUI::NewSettings);
        case Cosmos::WARNING_PAGE:
            return new (CosmosUI::MessagePageWindow);
        case Cosmos::VARIANT_SELECT:
            return new (CosmosUI::VariantSelectPlus);
        case Cosmos::SPINNER_WITH_BACKGROUND:
            return new (CosmosUI::AwaitPageWithBackground);
        case Aurora::LICENSE_CLASS_PROGRESS:
            return new (Aurora::UI::LicenseProgress);
        case Aurora::LEADERBOARDS_MAIN:
            return new (Aurora::UI::LeaderboardMain);
        default:
            return Scene::CreatePageById((PageId)pageId);
    }
    return nullptr;
}

kmCall(0x80622d2c, CreatePage);

void InjectLicenseSettingsPages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Aurora::LICENSE_CLASS_PROGRESS);
}
kmCall(0x8062d1cc, InjectLicenseSettingsPages);

void InjectWFCPages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
}
kmCall(0x8062dc4c, InjectWFCPages);
kmCall(0x8062ddf0, InjectWFCPages);
kmCall(0x8062df94, InjectWFCPages);
kmCall(0x8062e2b8, InjectWFCPages);
kmCall(0x8062e45c, InjectWFCPages);
kmCall(0x8062e600, InjectWFCPages);

void InjectOnlinePages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::VARIANT_SELECT);
}
kmCall(0x8062e864, InjectOnlinePages);
kmCall(0x8062e0d8, InjectOnlinePages);
kmCall(0x8062e744, InjectOnlinePages);
kmCall(0x8062eaa4, InjectOnlinePages);

void InjectMenuSinglePages(Scene& scene, PageId id)
{
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
    scene.CreatePage((PageId)Cosmos::VARIANT_SELECT);

    //DEV
    scene.CreatePage((PageId)Aurora::LEADERBOARDS_MAIN);

    return;
}

//MenuSingle
kmCall(0x8062d334, InjectMenuSinglePages); //From Main
kmCall(0x8062d478, InjectMenuSinglePages); //From Change Char
kmCall(0x8062d5bc, InjectMenuSinglePages); //From ChangeCourse
kmCall(0x8062d640, InjectMenuSinglePages); //From Versus
kmCall(0x8062d6c4, InjectMenuSinglePages); //From Battle
kmCall(0x8062d808, InjectMenuSinglePages); //From Mission MOde
//MenuMulti
kmCall(0x8062db5c, InjectMenuSinglePages);

void InjectGhostPages(Scene& scene, PageId id) {
    scene.CreatePage(TIME_TRIAL_INTERFACE);
    scene.CreatePage(SPLITS_AFTER_TT);


}

kmCall(0x8062ccd4, InjectGhostPages);
kmCall(0x8062cc5c, InjectGhostPages);
kmCall(0x8062cc98, InjectGhostPages);

void InjectTTPages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
    scene.CreatePage(ARE_YOU_SURE_YOU_WANT_TO_QUIT);
    scene.CreatePage(VOTERANDOM_MESSAGE_BOX);
    scene.CreatePage((PageId)Cosmos::SPINNER_WITH_BACKGROUND);
    scene.CreatePage(GENERIC_TEXT_BOX_FULL_PAGE_PRESS_A);
    scene.CreatePage(PAGE_MESSAGE_BOX_TRANSPARENT);
};

kmCall(0x8062c644, InjectTTPages); // TTs

void InjectWarningPage(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::WARNING_PAGE);
    return;
}

void InjectVSPausePages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
    scene.CreatePage(ARE_YOU_SURE_YOU_WANT_TO_QUIT);
}
void InjectVSPausePagesNoConfirm(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SETTINGS_MAIN);
}
kmCall(0x8062c65c, InjectVSPausePages);
kmCall(0x8062c7a0, InjectVSPausePages);

kmCall(0x8062c6a4, InjectVSPausePagesNoConfirm);
kmCall(0x8062c6f8, InjectVSPausePagesNoConfirm);
kmCall(0x8062c74c, InjectVSPausePagesNoConfirm);
kmCall(0x8062c7e8, InjectVSPausePagesNoConfirm);
kmCall(0x8062c83c, InjectVSPausePagesNoConfirm);
kmCall(0x8062c890, InjectVSPausePagesNoConfirm);
kmCall(0x8062c5d8, InjectVSPausePagesNoConfirm);

kmCall(0x8062ce54, InjectWarningPage);
kmCall(0x8062ced8, InjectWarningPage);
kmCall(0x8062cf5c, InjectWarningPage);
kmCall(0x8062cfe0, InjectWarningPage);
kmCall(0x8062d064, InjectWarningPage);

void InjectMainMenuPages(Scene& scene, PageId id){
    scene.CreatePage(id);
    scene.CreatePage((PageId)Cosmos::SPINNER_WITH_BACKGROUND);
    scene.CreatePage(GENERIC_TEXT_BOX_FULL_PAGE_PRESS_A);
    return;
}
kmCall(0x8062ce78, InjectMainMenuPages);
kmCall(0x8062cefc, InjectMainMenuPages);
kmCall(0x8062cf80, InjectMainMenuPages);
kmCall(0x8062d004, InjectMainMenuPages);
kmCall(0x8062d088, InjectMainMenuPages);


static int FontDotSize = 2;

asm int AdjustFontSize() {
    ASM (
        nofralloc;
        lis r6, FontDotSize@ha;
        lwz r6, FontDotSize@l (r6);
        mr r0, r6;
        blr;
    );
}
kmCall(0x80022278, AdjustFontSize);



