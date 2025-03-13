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
 * 
 * This code is utilitizing the /dev/dolphin IOS available in Dolphin Emulator
 * https://github.com/dolphin-emu/dolphin/blob/master/Source/Core/Core/IOS/DolphinDevice.cpp
 * 
 */

#include <kamek.hpp>
#include <Debug/IOSDolphin.hpp>
#include <core/egg/Fader.hpp>
#include <UI/CupSelect/CourseSelect.hpp>
#include <UI/Scene.hpp>
#include <UI/Settings/NewSettingsPage.hpp>
#include <UI/Settings/MessageWarning.hpp>
#include <UI/Leaderboards/LeaderboardMain.hpp>
#include <Aurora/AuroraLicense.hpp>


namespace Cosmos
{
    static bool isSpedUp = false;
    u32 SetDolphinSpeed(EGG::ColorFader* fader, u32, u32, u32 ret){
        if(IOS::Dolphin::IsOpen())
        {
            if(fader->status == EGG::Fader::CAN_FADE_IN) { 
                IOS::Dolphin::SetSpeedLimit(0);
            }

            else {
                IOS::Dolphin::SetSpeedLimit(100);
            }
        }

        return ret;
    }
    kmBranch(0x802153b0, SetDolphinSpeed);
} // namespace Cosmos

namespace Aurora {
    ScenePlus* CreateScenePlus(){
        return new ScenePlus;
    }
    kmWrite32(0x8063504c, 0x60000000);
    kmCall(0x80635058, CreateScenePlus);

    Page* ScenePlus::CreatePageWithId(PageId pageId){
        switch(pageId){
            case Aurora::SETTINGS_MAIN:
                return new (CosmosUI::NewSettings);
            case Aurora::WARNING_PAGE:
                return new (CosmosUI::MessagePageWindow);
            case Aurora::VARIANT_SELECT:
                return new (CosmosUI::VariantSelectPlus);
            case Aurora::SPINNER_WITH_BACKGROUND:
                return new (CosmosUI::AwaitPageWithBackground);
            case Aurora::LICENSE_CLASS_PROGRESS:
                return new (Aurora::UI::LicenseProgress);
            case Aurora::LEADERBOARDS_MAIN:
                return new (Aurora::UI::LeaderboardMain);
            case Aurora::LEADERBOARDS_DETAIL:
                return new (Aurora::UI::LeaderboardDetail);
            default:
                return Scene::CreatePageById((PageId)pageId);
        }
        return nullptr;
    }

    Page* ScenePlus::LoadAndInitPage(PageId pageId){
        Page* page = ScenePlus::CreatePageWithId(pageId);
        if(pageId < START_INDEX) this->Set(page, pageId);
        else this->SetAuroraPage(page, pageId);
        page->Setup(pageId);        
        return page;
    }

    Page* ScenePlus::AddActivePage(PageId pageId){
        Page* page;
        if(pageId < START_INDEX) page = this->Get<Page>(pageId);
        else page = this->GetAuroraPage<Page>(pageId);

        this->activePages[++this->layerCount] = page;
        if(this->openingAnimIdx != -1) page->animationDirection = this->openingAnimIdx;
        page->Activate();
        return page;
    }

    Page*  ScenePlus::ActivatePage(PageId pageId, s32 animationDirection){
        Page* page;
        if(pageId < START_INDEX) page = this->Get<Page>(pageId);
        else page = this->GetAuroraPage<Page>(pageId);

        this->activePages[++this->layerCount] = page;
        page->animationDirection = animationDirection;
        page->Activate();
        return page;
    }

    void ScenePlus::Deinit(){
        Scene::Exit();
        /*
        for(int i = 0; i < AURORA_PAGE_COUNT; i++){
            if(this->auroraPages[i]){
                this->auroraPages[i]->Dispose();
                delete this->auroraPages[i];
            }
        }
            */
    }
    //806221d8

    void PatchActivationInline(){
        register ScenePlus* scene;
        register PageId pageId;
        register u32 direction;
        asm{ASM(mr scene, r28; mr pageId, r29; mr direction, r26;)}
        scene->ActivatePage(pageId, direction);
    };
    kmCall(0x80623118, PatchActivationInline);
    kmWrite32(0x8062311c, 0x48000034);

    Page* Scene_LoadAndInitPage(ScenePlus* scene, PageId id){
        return scene->LoadAndInitPage(id);
    }
    kmBranch(0x80622d08, Scene_LoadAndInitPage);

    void Scene_AddActivePage(ScenePlus* scene, PageId id){
        scene->AddActivePage(id);
    }
    kmBranch(0x80622da0, Scene_AddActivePage);

    void Scene_ActivatePage(ScenePlus* scene, PageId id, u32 direction){
        scene->ActivatePage(id, direction);
    }
    kmBranch(0x80622e00, Scene_ActivatePage);

    void Scene_Deinit(ScenePlus* scene){
        scene->Deinit();
    }
    kmCall(0x806351d8, Scene_Deinit);
}
