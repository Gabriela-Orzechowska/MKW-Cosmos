#pragma once

#include "Network/RKNetController.hpp"
#include "Network/RKNetSelect.hpp"
#include "System/Identifiers.hpp"
#include "UI/Ctrl/PushButton.hpp"
#include "UI/Layout/ControlLoader.hpp"
#include "UI/Page/Other/CountDownTimer.hpp"
#include "UI/Page/Other/VR.hpp"
#include "System/System.hpp"
#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Menu/Menu.hpp>
#include <game/UI/Page/Other/LicenseSettings.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <Settings/UserData.hpp>
#include <UI/MiscUI.hpp>
#include <game/Network/RKNetUser.hpp>

namespace Aurora {
    namespace UI {
        struct AuroraPlayerInfo{
            u8 aid; //shared by players on the same console
            u8 hudSlotid; //0 for P1, 1 for P2 on the same console etc....
            u8 unknown_0x2[1]; //never stored likely padding
            u8 licenceClass;
            u32 team; //0x2 if no team (depending on the menu), 0x0 blue, 0x1 red 0x1F8
            u16 vr; //806519c4, no idea, gets the current license then does a bunch of operations 0x1FC
            u16 br; //80651a20, very similar, but uses a different address to start the operations
        };//total size0xC
        
        class LicenseSelectPlus : public Pages::LicenseSelect {
        public:
            ~LicenseSelectPlus() override {};
            void OnInit() override;
            void OnActivate() override;

            void OnLicenseButtonSelect(PushButton* button, u32 hudSlotId);

        private:
            PtmfHolder_2A<Page, void, PushButton*, u32> onSelectHandler;

            
        };

        class LicenseProgress : public Page {
        public:
            static const PageId pageId = (PageId) Aurora::LICENSE_CLASS_PROGRESS;

            LicenseProgress();
            void OnInit() override;
            void OnActivate() override;
            PageId GetNextPage() const override { return LICENSE_SETTINGS; };

            void OnBack(u32 hudSlotId) { EndStateAnimate(0.0, 1); }
            void OnBackButtonClick(PushButton * button, u32 hudSlotId) { EndStateAnimate(0.0f, 1); }

        private:
            LicenseControl licenses[3];

            ControlsManipulatorManager controlsManipulatorManager;
            CtrlMenuPageTitleText titleText;
            CtrlMenuBackButton backButton;
            CtrlMenuInstructionText bottomText;
            MiiGroup* miiGroup;
            PageId nextPageId;

            PtmfHolder_1A<Page, void, u32> onBackPressHandler;       
            PtmfHolder_2A<Page, void, PushButton*, u32> onBackButtonPress; 
        };
    }
}
