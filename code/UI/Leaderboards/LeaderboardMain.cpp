#include "Aurora/AuroraAPI.hpp"
#include "GlobalFunctions.hpp"
#include "Race/RaceData.hpp"
#include "System/Identifiers.hpp"
#include "System/SaveDataManager.hpp"
#include "System/System.hpp"
#include "System/identifiers.hpp"
#include "UI/Ctrl/Manipulator.hpp"
#include "UI/Page/Other/GhostSelect.hpp"
#include "Visual/Mii.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include <UI/Leaderboards/LeaderboardMain.hpp>
#include <UI/BMG/BMG.hpp>
#include <UI/MiscUI.hpp>
#include <UI/Scene.hpp>
#include <game/UI/Page/Menu/StartRace.hpp>

extern "C" char* vehicle_names[1]; //808B3B50

namespace Aurora {
    namespace UI {

        void LeaderboardEntry::Load(int index){
            char buffer[0x20];
            snprintf(buffer, 0x20, "LeaderboardEntry_%d", index);
            PushButton::Load("button", "AuroraLeaderboardEntryButton", buffer, 1, 0, false);
            this->buttonId = 4+index;
            this->LoadPictureLayout("control", "chara_flag_machine_picture_common");
        }

        void LeaderboardEntry::SetData(int position, MiiGroup* group, int miiIndex, CharacterId chara, KartId kart, u32 minutes, u32 seconds, u32 millis,
                u32 flag, u32 controller){
            SetTextBoxMsg("text_position", 0x1839 + position, nullptr);
            SetMiiPane("mii_icon", group, miiIndex, 0);

            TextInfo info;
            info.miis[0] = group->GetMii(miiIndex);
            SetTextBoxMsg("text", 0x251d, &info);
            SetTextBoxMsg("text_light_01", 0x251d, &info);
            SetTextBoxMsg("text_light_02", 0x251d, &info);

            info.intToPass[0] = minutes;
            info.intToPass[1] = seconds;
            info.intToPass[2] = millis;
            SetTextBoxMsg("text_time", 0x17A4, &info);

            char buffer[0x40];
            snprintf(buffer, sizeof(buffer), "Vehicle%02d", kart);
            this->SetPicturePane("vehicle_icon", buffer);
            this->SetPicturePane("character_icon", GetCharacterIconPaneName(chara));
            this->SetPaneVisible("controller_icon", controller < 4);
            if(controller < 4){
                snprintf(buffer, sizeof(buffer), "button/timg/f_controller_%02d.tpl", controller);
                void* tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, buffer, 0);
                CosmosUI::ChangePaneImage(this, "controller_icon", tplPointer);

            }
            snprintf(buffer, sizeof(buffer), "%03d", flag);
            if(this->PicturePaneExists(buffer)){
                this->SetPaneVisible("flag_icon", true);
                this->SetPicturePane("flag_icon", buffer);
            }
            else {
                this->SetPaneVisible("flag_icon", false);

            }
        }

        LeaderboardMain::LeaderboardMain() {
            SetupHandler(onBackPressHandler, void (Page::*)(u32), &LeaderboardMain::OnBack);
            SetupHandler(onBackButtonPress, void (Page::*)(PushButton*,u32), &LeaderboardMain::OnBackButtonClick);
            SetupHandler(onButtonPress, void (Page::*)(PushButton*,u32), &LeaderboardMain::OnButtonClick);
            SetupHandler(onButtonSelect, void (Page::*)(PushButton*,u32), &LeaderboardMain::OnButtonSelect);
            nextPageId = (PageId)SINGLE_PLAYER_MENU;
        };

        void LeaderboardMain::OnInit() {
            Page::OnInit();
            this->miiGroup.Init(0x14, 0x1, nullptr);
            //this->miiGroup.LoadStoreMii(0, &SaveDataManager::GetStaticInstance()->GetCurrentLicense().miiId);

            this->controlsManipulatorManager.Init(0x1, false);
            this->controlsManipulatorManager.SetDistanceFunc(3);
            this->SetManipulatorManager(&this->controlsManipulatorManager);
            this->animationDelay = 5.0f;

            this->InitControlGroup(4 + buttonCount);

            this->AddControl(0, &this->backButton, false);
            this->AddControl(1, &this->bottomText, false);
            this->AddControl(2, &this->titleText, false);
            this->AddControl(3, &this->pageSelector, false);
            for(int i = 0; i < buttonCount; i++){
                this->AddControl(4+i, &this->entry[i], false);
            }
            this->titleText.Load(false);

            this->backButton.Load("button", "Back", "ButtonBack", 1, false, true);
            this->bottomText.Load();
            this->pageSelector.Load(1, 0, "control", "AuroraLeaderboardPageUpDownBase", "UpDown4", 
                    "AuroraLeaderboardPageUpDownButtonR", "RightButton","AuroraLeaderboardPageUpDownButtonL", "LeftButton", 
                    (UpDownDisplayedText*) &this->textPageSelector, 1, 0, false, true, true);
            this->textPageSelector.Load("control", "AuroraLeaderboardPageUpDownValue", "Value", "AuroraLeaderboardPageUpDownText", "Text");

            for(int i = 0; i < buttonCount; i++){
                this->entry[i].Load(i);

                this->entry[i].SetOnClickHandler(&this->onButtonPress,0);
                this->entry[i].SetOnSelectHandler(&this->onButtonSelect);
            }
            this->entry[0].SelectInitialButton(0);
            this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, &this->onBackPressHandler, false, false);

            this->backButton.SetOnClickHandler(&this->onBackButtonPress, 0);
            this->titleText.SetMsgId(0x0);
        };

        void LeaderboardMain::UpdateData() {
            API::APILeaderboards* leaderboards = API::Manager::GetStaticInstance()->GetLeaderboards();
            if(buttonCount > leaderboards->entryCount){
                for(int i = 0; i < buttonCount; i++){
                    if((i >= leaderboards->entryCount)){
                        this->entry[i].isHidden = true;
                        this->entry[i].SetPlayerBitfield(0);
                    }
                    else {
                        this->entry[i].isHidden = false;
                        this->entry[i].SetPlayerBitfield(1);
                    }
                }
            }
            u32 entriesToDisplay = buttonCount;
            if(leaderboards->entryCount < buttonCount) entriesToDisplay = leaderboards->entryCount;
            for(int i = 0; i < entriesToDisplay; i++){
                leaderboards->entries[i].miicrc = RFLiCalculateCRC(&leaderboards->entries[i].miiData, 0x4C);
                this->miiGroup.AddMii(i, &leaderboards->entries[i].miiData);
            }
            for(int i = 0; i < entriesToDisplay; i++){
                u32 time = leaderboards->entries[i].timeInMs;
                u32 millis = time % 1000;
                u32 seconds = (time / 1000) % 60;
                u32 minutes = (time / 60000);
                CosmosLog("Country: %d, Controller: %d\n", leaderboards->entries[i].flag, leaderboards->entries[i].controller);
                this->entry[i].SetData(i, &this->miiGroup, i, (CharacterId) leaderboards->entries[i].chara, 
                        (KartId) leaderboards->entries[i].vehicle, minutes, seconds, millis,
                        leaderboards->entries[i].flag, leaderboards->entries[i].controller);
            }
        }

        void LeaderboardMain::OnActivate(){
            Page::OnActivate();
            if(API::Manager::GetStaticInstance()->HasLeaderboardsUpdate()) {
                this->UpdateData();
                API::Manager::GetStaticInstance()->MarkLeaderboardsUpdated();
            }
            this->titleText.SetMsgId(0x1777, nullptr);
        }

        void LeaderboardMain::OnButtonClick(PushButton* button, u32){
            LeaderboardDetail* details = LeaderboardDetail::GetPage();
            u32 id = button->buttonId - 4;
            details->FillData(&API::Manager::GetStaticInstance()->GetLeaderboards()->entries[id], &this->miiGroup, id, 371);
            this->nextPageId = (PageId) LEADERBOARDS_DETAIL;
            this->EndStateAnimate(button->GetAnimationFrameSize(), 0);
        }

        LeaderboardDetail::LeaderboardDetail() {
            SetupHandler(onBackPressHandler, void (Page::*)(u32), &LeaderboardDetail::OnBack);
            SetupHandler(onBackButtonPress, void (Page::*)(PushButton*,u32), &LeaderboardDetail::OnBackButtonClick);
            SetupHandler(onChallengePress, void (Page::*)(PushButton*,u32), &LeaderboardDetail::OnChallengePress);
            //SetupHandler(onButtonSelect, void (Page::*)(PushButton*,u32), &LeaderboardMain::OnButtonSelect);
            nextPageId = (PageId) LEADERBOARDS_MAIN;
            state = 0;

        }

        void LeaderboardDetail::OnInit() {
            this->controlsManipulatorManager.Init(0x1, false);
            this->controlsManipulatorManager.SetDistanceFunc(3);
            this->SetManipulatorManager(&this->controlsManipulatorManager);

            this->InitControlGroup(6);

            this->AddControl(0, &this->backButton, false);
            this->AddControl(1, &this->bottomText, false);
            this->AddControl(2, &this->titleText, false);
            this->AddControl(3, &this->infoControl, false);
            this->AddControl(4, &this->buttonWatch, false);
            this->AddControl(5, &this->buttonChallenge, false);
            this->titleText.Load(false);

            this->backButton.Load("button", "Back", "ButtonBack", 1, false, true);
            this->bottomText.Load();

            this->buttonWatch.Load("button", "TimeAttackGhostList", "Watch", 1, 0, false);
            this->buttonWatch.SetOnClickHandler(&this->onChallengePress, 0);
            this->buttonChallenge.Load("button", "TimeAttackGhostList", "Alone", 1, 0, false);
            this->buttonChallenge.SetOnClickHandler(&this->onChallengePress, 0);

            this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, &this->onBackPressHandler, false, false);
            this->infoControl.Load("GhostBattleInfo");

            this->backButton.SetOnClickHandler(&this->onBackButtonPress, 0);
            this->titleText.SetMsgId(0x0);
            this->buttonWatch.SelectInitialButton(0);
        };

        void LeaderboardDetail::OnResume(){
            if(state == 0) return;
            else if(state == 1){
                state = 3;
                this->AddPageLayer(START_RACE, 0);
            }
            else if(state == 3){
                Pages::StartRace* start = Pages::StartRace::GetPage();
                if(start->hasButtonBeenPressed) {
                    for(int i = 1; i < 12; i++){
                        RaceData::GetStaticInstance()->menusScenario.GetPlayer(i).playerType = PLAYER_NONE;
                    }
                    RaceData::GetStaticInstance()->menusScenario.GetPlayer(0).playerType = PLAYER_GHOST;
                    Pages::GhostManager *ghostManager = Pages::GhostManager::GetPage();
                    if (ghostManager->state == SAVED_GHOST_RACE_FROM_MENU)
                        ghostManager->state = STAFF_GHOST_RACE_FROM_MENU;
                    ghostManager->list.InitMenudata98Params(0);
                    ghostManager->SetupGhostReplay(true);
                    Cosmos::Ghost::GhostManager::GetStaticInstance()->LoadGhostReplay(ghostManager->rkgPointer, false);
                    this->ChangeMenu(WATCH_GHOST_FROM_MENU, 0, 0.0f);
                    this->nextPageId = PAGE_NONE;
                }
                else state = 0;
            }
        }

        void LeaderboardDetail::FillData(API::APILeaderboardEntry* entry, MiiGroup* group, u32 miiIndex, u32 slot){
            this->currentEntry = entry;
            this->infoControl.SetTextBoxMsg("course_name", slot + BMG_TRACKS, nullptr);
            TextInfo info;
            u32 time = entry->timeInMs;
            info.intToPass[0] = (time / 60000);
            info.intToPass[1] = (time / 1000) % 60;
            info.intToPass[2] = (time) % 1000;
            this->infoControl.SetTextBoxMsg("time", 0x17a4, &info);
            info.miis[0] = group->GetMii(miiIndex);
            this->infoControl.SetTextBoxMsg("player", 0x251d, &info);
            char buffer[0x20];
            snprintf(buffer, sizeof(buffer), "%03d", entry->flag);
            if(this->infoControl.PicturePaneExists(buffer)){
                this->infoControl.SetPaneVisible("nationality", true);
                this->infoControl.SetPicturePane("nationality", buffer);
            }
            else {
                this->infoControl.SetPaneVisible("nationality", false);
            }
            this->infoControl.SetPaneVisible("nintendo", false);
            this->infoControl.SetMiiPane("mii", group, miiIndex, 0);
            u32 controller = entry->controller;
            this->infoControl.SetPaneVisible("handle", controller < 4);
            if(controller < 4){
                snprintf(buffer, sizeof(buffer), "button/timg/f_controller_%02d.tpl", controller);
                void* tplPointer = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, buffer, 0);
                CosmosUI::ChangePaneImage(&this->infoControl, "handle", tplPointer);
            }
            snprintf(buffer, sizeof(buffer), "Vehicle%02d", entry->vehicle);
            this->infoControl.SetPicturePane("machine", buffer);
            this->infoControl.SetPicturePane("chara", GetCharacterIconPaneName((CharacterId)entry->chara));
        }

        void LeaderboardDetail::OnActivate() {
            Page::OnActivate();
        }

        static void OnDownloadCallback(void* arg, API::DownloadError error){
            Pages::AutoEnding* ending = MenuData::GetStaticInstance()->GetAuroraPage<Pages::AutoEnding>(SPINNER_WITH_BACKGROUND);
            ending->Pop();
            if (error == API::NONE)
                ((LeaderboardDetail*)arg)->ConfirmSuccess();
            else
                ((LeaderboardDetail*)arg)->ConfirmFail();
        }

        void LeaderboardDetail::OnChallengePress(PushButton* button, u32 hudslotid){
            (void)button;
            (void)hudslotid;

            this->AddPageLayer((PageId)Aurora::SPINNER_WITH_BACKGROUND, 0);
            API::Manager::GetStaticInstance()->StartDownloadGhost(this->currentEntry, OnDownloadCallback, this);
        }

        int FixDistanceFunc(ControlManipulator *main, ControlManipulator *dest, Directions direction, 
                            bool horizontalWrap, bool verticalWrap){
            u32 buttonId = ((CtrlMenuSingleTopMovieButton*)(main->actionHandlers[0]->subject))->buttonId;
            u32 destId = ((CtrlMenuSingleTopMovieButton*)(dest->actionHandlers[0]->subject))->buttonId;

            if(buttonId == 1 && destId == 4 && direction == DIRECTION_UP) return 1;
            else if(buttonId == 4 && destId == 1 && direction == DIRECTION_DOWN) return 1;
            else if(buttonId == 4 && direction == DIRECTION_UP) return 0;

            return main->CalcDistanceVerticalWrappingOnly(dest, direction);
        }
        GhostSelectPlus* CreateGhostSelectPage(){
            return new GhostSelectPlus;
        }

        void GhostSelectPlus::OnInit(){
            Pages::GhostSelect::OnInit();
            this->AddControl(9, &this->leaderboardsButton, false);
            this->leaderboardsButton.Load("button", "DriftHelp", "DriftHelp", 1, 0, false);
            this->challengeGhostButton.buttonId = 1;
            this->leaderboardsButton.buttonId = 4;
            this->leaderboardsButton.SetMsgId(0x0c1f, nullptr);
            this->leaderboardsButton.SetOnClickHandler(&this->onLeaderboardsButton, 0);

            this->pageNumber.isHidden = true;
            this->manipulatorManager.calcDistanceFunc = &FixDistanceFunc;
        }
        void GhostSelectPlus::OnActivate(){
            Pages::GhostSelect::OnActivate();
            this->pageNumber.isHidden = true;

        }

        void GhostSelectPlus::OnLeaderboardsButton(PushButton* button, u32 hudslotid){
            (void)button;
            (void)hudslotid;

            this->nextPageId = Aurora::API::Manager::GetStaticInstance()->LoadLeaderboardsPage(this, 371);
            this->EndStateAnimate(button->GetAnimationFrameSize(), 0);

        }

        //kmCall(0x80623dec, CreateGhostSelectPage);
        //kmWrite32(0x80623de0, 0x60000000);
        kmWrite8(0x80639958 + 3, 10);
    };
};
