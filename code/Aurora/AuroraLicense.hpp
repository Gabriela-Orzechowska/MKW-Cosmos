#pragma once

#include "Network/RKNetController.hpp"
#include "Network/RKNetSelect.hpp"
#include "System/Identifiers.hpp"
#include "UI/Layout/ControlLoader.hpp"
#include "UI/Page/Other/CountDownTimer.hpp"
#include "UI/Page/Other/VR.hpp"
#include "main.hpp"
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
