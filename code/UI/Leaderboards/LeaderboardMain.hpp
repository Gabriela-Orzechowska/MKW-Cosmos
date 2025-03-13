#include "Aurora/AuroraAPI.hpp"
#include "System/Identifiers.hpp"
#include "System/identifiers.hpp"
#include "UI/Ctrl/GhostInfoControl.hpp"
#include "UI/Ctrl/PushButton.hpp"
#include "UI/Ctrl/UIControl.hpp"
#include "UI/MenuDataPlus.hpp"
#include "UI/Page/Other/GhostManager.hpp"
#include <game/UI/Ctrl/UpDown.hpp>
#include "UI/Page/Other/GhostSelect.hpp"
#include "Visual/Mii.hpp"
#include "core/nw4r/lyt/Picture.hpp"
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <System/System.hpp>

namespace Aurora {
    namespace UI{

        class LeaderboardEntry : public PushButton {
        public:
            LeaderboardEntry() {};
            ~LeaderboardEntry() {};

            void Load(int index);
            void SetData(int position, MiiGroup* group, int miiIndex, CharacterId chara, KartId kartId, u32 minutes, u32 seconds, u32 millis,
                    u32 flag = 0, u32 controller = 0);

        };

        class LeaderboardMain : public Page {
        public:

#define buttonCount 7
            static const PageId pageId = (PageId) Aurora::LEADERBOARDS_MAIN;

            LeaderboardMain();
            void OnInit() override;
            void OnActivate() override;
            PageId GetNextPage() const override { return nextPageId; };

            void OnBack(u32 hudSlotId) { 
                nextPageId = GHOST_SELECT;
                EndStateAnimate(0.0, 1); }
            void OnBackButtonClick(PushButton * button, u32 hudSlotId) {
                nextPageId = GHOST_SELECT;
                EndStateAnimate(0.0f, 1); 
            }

            void OnButtonClick(PushButton* button, u32 hudSlotId);
            void OnButtonSelect(PushButton* button, u32 hudSlotId) { return; }

            void UpdateData();

        private:
            LeaderboardEntry entry[buttonCount];
            UpDownControl pageSelector; //0x4CC
            TextUpDownValueControl textPageSelector; //0xA94
            ControlsManipulatorManager controlsManipulatorManager;
            CtrlMenuPageTitleText titleText;
            CtrlMenuBackButton backButton;
            CtrlMenuInstructionText bottomText;
            MiiGroup miiGroup;
            PageId nextPageId;

            PtmfHolder_1A<Page, void, u32> onBackPressHandler;       
            PtmfHolder_2A<Page, void, PushButton*, u32> onBackButtonPress; 
            PtmfHolder_2A<Page, void, PushButton*, u32> onButtonPress; 
            PtmfHolder_2A<Page, void, PushButton*, u32> onButtonSelect; 
        };
        class LeaderboardDetail : public Page {
        public:
            static const PageId pageId = (PageId) Aurora::LEADERBOARDS_DETAIL;
            static inline LeaderboardDetail* GetPage() { return MenuData::GetStaticInstance()->GetAuroraPage<LeaderboardDetail>(pageId);}
            LeaderboardDetail();
            void OnInit() override;
            void OnActivate() override;
            void OnResume() override;
            PageId GetNextPage() const override { return nextPageId; };

            void OnBack(u32 hudSlotId) { EndStateAnimate(0.0, 1); }
            void OnBackButtonClick(PushButton * button, u32 hudSlotId) { EndStateAnimate(0.0f, 1); }

            void OnChallengePress(PushButton*, u32);

            void FillData(API::APILeaderboardEntry* entry, MiiGroup* group, u32 miiIndex, u32 slot);
            inline void ConfirmSuccess() {
                state = 1;
            }
            inline void ConfirmFail() {
                state = 2;
            }
        protected:
            PageId nextPageId;
            ControlsManipulatorManager controlsManipulatorManager;
            CtrlMenuPageTitleText titleText;
            CtrlMenuBackButton backButton;
            CtrlMenuInstructionText bottomText;
            GhostInfoControl infoControl;
            PushButton buttonWatch;
            PushButton buttonChallenge;
            PtmfHolder_1A<Page, void, u32> onBackPressHandler;       
            PtmfHolder_2A<Page, void, PushButton*, u32> onBackButtonPress; 
            PtmfHolder_2A<Page, void, PushButton*, u32> onChallengePress; 
            API::APILeaderboardEntry* currentEntry;

            u32 state;

        };

        class GhostSelectPlus : public Pages::GhostSelect {
        public: 
            GhostSelectPlus() : temp(false) {
                SetupHandler(onLeaderboardsButton, void (Page::*)(PushButton*,u32), &GhostSelectPlus::OnLeaderboardsButton);
            };
            ~GhostSelectPlus() override {}

            void OnInit() override;
            void OnActivate() override;

            void OnLeaderboardsButton(PushButton* button, u32 hudslotid);


        protected:
            bool temp;
            PushButton leaderboardsButton;
            PtmfHolder_2A<Page, void, PushButton*, u32> onLeaderboardsButton;
        };
    }
}
