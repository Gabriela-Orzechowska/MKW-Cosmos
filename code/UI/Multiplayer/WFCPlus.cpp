#include <UI/Multiplayer/WFCPlus.hpp>
#include "Aurora/AuroraAPI.hpp"
#include "UI/BMG/BMG.hpp"
#include "UI/MenuData/MenuData.hpp"
#include "UI/Settings/NewSettingsPage.hpp"
#include "UI/Text.hpp"
#include "kamek.hpp"
#include "main.hpp"

namespace Cosmos {
    namespace UI {
        WFCMainPlus* CreateWFCPage(){
            return new (WFCMainPlus);
        }
        kmCall(0x8062405c, CreateWFCPage);
        kmWrite32(0x80624050, 0x60000000);

        void WFCMainPlus::OnSettingsClick(PushButton* button, u32 hudSlotId){
            CosmosUI::NewSettings* settings = CosmosUI::NewSettings::GetPage();
            COSMOS_ASSERT_NOT_NULL(settings);

            MenuId currentMenu = MenuData::GetStaticInstance()->GetCurrentScene()->menuId;
            settings->SetPreviousPage(WFC_MAIN_PAGE, currentMenu);

            this->nextPageId = (PageId) Cosmos::SETTINGS_MAIN;

            this->EndStateAnimate(0.0f, 0);
        }

        void WFCMainPlus::OnInit(){
            Pages::WFCMain::OnInit();
            TextInfo info;
            info.intToPass[0] = -1;

            this->continentalButton.SetTextBoxMsg("go", 0x284B, &info);
        }

        void PatchWorldwideButton(PushButton& button, const char* folder, const char* file,
                const char* variant){
            button.Load(folder, "CosmosWifiMenuSingleTop", variant, 1,0,false);
        }
        kmCall(0x8064ba38, PatchWorldwideButton);

        void PatchContinentalButton(PushButton& button, const char* folder, const char* file,
                const char* variant){
            button.Load(folder, "CosmosWifiMenuSingleTopContinental", variant, 1,0,false);
        }
        kmCall(0x8064ba90, PatchContinentalButton);

        void PatchFriendsButton(PushButton& button, const char** anims, const char* folder, const char* file,
                const char* variant){
            button.LoadWithAnims(anims, folder, "CosmosWifiMenuSingleTopFriendButton", variant, 1, 0);
        }
        kmCall(0x8064bb44, PatchFriendsButton);



        void WFCMainPlus::OnActivate(){
            if(this->animationDirection == 0){
                this->continentalButton.SelectInitialButton(0);
                this->bottomText.SetMsgId(0x10d7);
            }
            this->nextPageId = PAGE_NONE;

            Aurora::API::Manager::GetStaticInstance()->GetPlayerCount(&WFCMainPlus::GetPlayerCountCallback, this);
        }

        void WFCMainPlus::GetPlayerCountCallback(void* arg, u32 val){
            COSMOS_ASSERT_NOT_NULL(arg); 
            WFCMainPlus* page = (WFCMainPlus*)arg;

            TextInfo info;
            info.intToPass[0] = val;

            page->continentalButton.SetTextBoxMsg("go", 0x284B, &info);
        };

        void WFCMainPlus::OnMyButtonSelect(PushButton* button, u32){
            u32 bmgId = 0x10d5 + button->buttonId;
            if(button->buttonId == 1) bmgId = BMG_SETTINGS_PAGE_BOTTOM - 1;
            if(button->buttonId == 0) bmgId = 0;
            this->bottomText.SetMsgId(bmgId);
        }
    }
}
