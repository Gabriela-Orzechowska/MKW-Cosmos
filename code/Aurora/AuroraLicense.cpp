#include "Settings/UserData.hpp"
#include "UI/Layout/ControlLoader.hpp"
#include "UI/Page/Other/LicenseSettings.hpp"
#include "hooks.hpp"
#include "main.hpp"
#include <Aurora/AuroraLicense.hpp>

extern "C" {
    u64 DWC_CreateFriendKey(void* val);
}

namespace Aurora {
    namespace UI {

        inline void SetPaneVTXColors(Pane* root, const char* name, u32 vtx1, u32 vtx2, u32 vtx3, u32 vtx4){
            Picture* pane = (Picture*)root->FindPaneByName(name, true);
            if(pane == nullptr) return;
            pane->vertexColours[0] = vtx1;
            pane->vertexColours[1] = vtx2;
            pane->vertexColours[2] = vtx3;
            pane->vertexColours[3] = vtx4;
        }

        inline void SetTextBoxColors(Pane* root, const char* name, u32 col1, u32 col2){
            TextBox* pane = (TextBox*)root->FindPaneByName(name, true);
            if(pane == nullptr) return;

            GXColorS10 acol1;
            acol1.r = ((col1 >> 24) & 0xFF);
            acol1.g = ((col1 >> 16) & 0xFF);
            acol1.b = ((col1 >> 8) & 0xFF);
            acol1.a = ((col1 & 0xFF));

            GXColorS10 acol2;
            acol2.r = (col2 >> 24) & 0xFF;
            acol2.g = (col2 >> 16) & 0xFF;
            acol2.b = (col2 >> 8) & 0xFF;
            acol2.a = (col2 >> 0) & 0xFF;

            pane->GetMaterial()->tevColours[0] = acol2;
            pane->GetMaterial()->tevColours[1] = acol1;
        }
        // BOTTOM PART, PATTERN 1, 2, TEXT 1, 2
        const u32 licenseColors[][7] = {
            {0x7debf5FF, 0x5c7b7dFF, 0x000000FF, 0xf0feffFF, 0x02303300},
            {0x00a1e6FF, 0x0477b5FF, 0x000000FF, 0xe3f8ffFF, 0x005a7300},
            {0x0EB573FF, 0x0E6D50FF, 0x000000FF, 0xd6ffefFF, 0x013b2d00},
            {0x09C631FF, 0x25841AFF, 0x000000FF, 0xf4fff2FF, 0x093b0000},
            {0x930FC1FF, 0x5C0E82FF, 0x000000FF, 0xf8ebffFF, 0x17002400},
            {0xF257BFFF, 0x991A8CFF, 0x000000FF, 0xfff0fdFF, 0x30032a00},
            {0xAAAAAAFF, 0xBBBBBBFF, 0x000000FF, 0xFFFFFFFF, 0x00000000},
        };

        LicenseProgress::LicenseProgress() {
            SetupHandler(onBackPressHandler, void (Page::*)(u32), &LicenseProgress::OnBack);
            SetupHandler(onBackButtonPress, void (Page::*)(PushButton*,u32), &LicenseProgress::OnBackButtonClick);
        };

        void LicenseProgress::OnInit() {
        // Init Manipulator
            this->controlsManipulatorManager.Init(0x1, false);
            this->controlsManipulatorManager.SetDistanceFunc(3);
            this->SetManipulatorManager(&this->controlsManipulatorManager);

            this->InitControlGroup(3 + 3);

            this->AddControl(0, &this->backButton, false);
            this->AddControl(1, &this->bottomText, false);
            this->AddControl(2, &this->titleText, false);
            for(int i = 0; i < 3; i++){
                this->AddControl(3+i, &this->licenses[0+i], false);
            }
            this->titleText.Load(false);

            this->backButton.Load("button", "Back", "ButtonBack", 1, false, true);

            this->bottomText.Load();

            this->controlsManipulatorManager.SetGlobalHandler(BACK_PRESS, &this->onBackPressHandler, false, false);
            this->backButton.SetOnClickHandler(&this->onBackButtonPress, 0);
            this->titleText.SetMsgId(0x0);

            const char* anims[] = {"Loop", "Loop", NULL, NULL};

            for(int i = 0; i < 3; i++){
                char buffer[0x20];
                snprintf(buffer, 0x20, "License_%d", i);
                ControlLoader lod = ControlLoader(&this->licenses[i]);
                lod.Load("control", "AuroraLicenseProgress", buffer, anims);

                this->licenses[i].animator.GetAnimationGroupById(0)->PlayAnimationAtFrame(0.0f, 0);
                /*
                this->licenses[i].animator.GetAnimationGroupById(1)->PlayAnimationAtFrame(0.0f, 0);
                this->licenses[i].animator.GetAnimationGroupById(2)->PlayAnimationAtFrame(0.0f, 0);
                this->licenses[i].animator.GetAnimationGroupById(3)->PlayAnimationAtFrame(0.0f, 0);
                */
            }
            this->backButton.SelectInitialButton(0);
        };

        void LicenseProgress::OnActivate() {
            if(this->miiGroup == nullptr){
                this->miiGroup = &Pages::LicenseSettings::GetPage()->miiGroup;
            }

            this->titleText.SetMsgId(0x2081A, nullptr);
            this->bottomText.SetMsgId(0x2081B, nullptr);

            for(int i = 0; i < 3; i++){
                LicenseControl& license = this->licenses[i];
                license.SetMiiPane("mii", this->miiGroup, 0, 0);

                u32 licenseClass = 6;
                if(i == 0) 
                    licenseClass = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetGPClass();
                else if (i == 1)
                    licenseClass = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetOnlineClass();

                char buffer[0x10];
                snprintf(buffer, 0x10, "license_%d.tpl", licenseClass);
                if(licenseClass != 6)
                    CosmosUI::ChangePaneImage(&license, "license_plate", buffer);
                else 
                    license.SetPaneVisible("license_plate", false);

                license.SetTextBoxMsg("player", 0x2807 + i, nullptr);

                Pane* rootPane = license.layout.layout.rootPane;
                Pane* colorful = rootPane->FindPaneByName("colorful_base", true);

                if(colorful != nullptr){
                    colorful->GetMaterial()->tevKColours[0] = licenseColors[licenseClass][0];
                }
                SetPaneVTXColors(rootPane, "mii_base", licenseColors[licenseClass][1], 0x000000FF, 0x000000FF, licenseColors[licenseClass][1]);
                SetPaneVTXColors(rootPane, "license_base", licenseColors[licenseClass][1], licenseColors[licenseClass][2],
                        licenseColors[licenseClass][1], licenseColors[licenseClass][2]);
                SetPaneVTXColors(rootPane, "player_base", licenseColors[licenseClass][1], licenseColors[licenseClass][2],
                        licenseColors[licenseClass][1], licenseColors[licenseClass][2]);

                SetTextBoxColors(rootPane, "player", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "stats_title", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "stats_value", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "req1_title", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "req1_value", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "req2_title", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "req2_value", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
            }


// GP LICENSE
            this->licenses[0].SetTextBoxMsg("stats_title", 0x20820, nullptr);
            this->licenses[0].SetTextBoxMsg("req1_title", 0x20822, nullptr);
            this->licenses[0].SetTextBoxMsg("req2_title", 0x20823, nullptr);

            TextInfo offlineInfo;
            u32 trophyPoints = 0;
            u32 completedCups = 0;
            u32 averageRank = 0;
            for(int j = 0; j < 4; j++){
                for(int i = 0; i < Cosmos::CupManager::GetStaticInstance()->GetMaxCupCount(); i++){
                    if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsGPCompleted(i, j)){
                        completedCups++;
                        trophyPoints += (3 - Cosmos::Data::SettingsHolder::GetStaticInstance()->GetGPTrophy(i, j)); 
                        averageRank += Cosmos::Data::SettingsHolder::GetStaticInstance()->GetGPRank(i, j);
                    }
                }
            }


            offlineInfo.intToPass[1] = Cosmos::CupManager::GetStaticInstance()->GetMaxCupCount() * 3 * 4;
            offlineInfo.intToPass[0] = trophyPoints;

            this->licenses[0].SetTextBoxMsg("stats_value", 0x20833, &offlineInfo);


            u32 curLicense = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetGPClass();
            if(curLicense != 5)
            {
                Cosmos::Data::LicenseClassRequirements& reqs = Cosmos::Data::globalClassRequirements[curLicense];

                u32 bmgId = 0x20830;
                if(completedCups > 0)
                    averageRank /= completedCups;
                else
                    averageRank = 8;

                offlineInfo.intToPass[0] = completedCups;
                offlineInfo.intToPass[1] = reqs.gpCompletedCups * Cosmos::CupManager::GetStaticInstance()->GetMaxCupCount() * 3 / 100;

                bmgId = completedCups >= offlineInfo.intToPass[1] ? 0x20831 : 0x20830;
                this->licenses[0].SetTextBoxMsg("req1_value", bmgId, &offlineInfo);

                wchar_t* rankArray[] = {
                    L"\uF063", L"\uF062", L"\uF061", L"\uF078",
                    L"\uF079", L"\uF07A", L"\uF07B", L"\uF07C",
                    L"?",
                };

                offlineInfo.strings[0] = rankArray[averageRank];
                offlineInfo.strings[1] = rankArray[reqs.gpAverageRank];

                this->licenses[0].SetTextBoxMsg("req2_value", 0x20832, &offlineInfo);
            }

// ONLINE LICENSE
            this->licenses[1].SetTextBoxMsg("stats_title", 0x20824, nullptr);
            this->licenses[1].SetTextBoxMsg("req1_title", 0x20826, nullptr);
            this->licenses[1].SetTextBoxMsg("req2_title", 0x20827, nullptr);

            TextInfo onlineInfo;
            u32 bmgId = 0x20830;
            onlineInfo.intToPass[0] = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetUserVR();

            this->licenses[1].SetTextBoxMsg("stats_value", 0x20825, &onlineInfo);

            curLicense = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetOnlineClass();
            if(curLicense != 5)
            {
                Cosmos::Data::LicenseClassRequirements& reqs = Cosmos::Data::globalClassRequirements[curLicense];

                u32 val = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetOnlineRaces();
                onlineInfo.intToPass[0] = val;
                onlineInfo.intToPass[1] = reqs.onlineCompletedRaces;

                bmgId = val >= reqs.onlineCompletedRaces ? 0x20831 : 0x20830;
                this->licenses[1].SetTextBoxMsg("req1_value", bmgId, &onlineInfo);

                val = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetOnlineScore();
                onlineInfo.intToPass[0] = val;
                onlineInfo.intToPass[1] = reqs.onlineMinScore;

                bmgId = val >= reqs.onlineMinScore ? 0x20831 : 0x20830;
                this->licenses[1].SetTextBoxMsg("req2_value", bmgId, &onlineInfo);
            }

        };

        // TO REMOVE
        kmWrite16(0x805edc5c + 2, Aurora::LICENSE_CLASS_PROGRESS);


        void LoadNewLicenseButton(PushButton& button, const char* folderName, const char* file, const char* variant){
            if(strcmp(file, "LicenseSelectT") == 0) button.Load(folderName, "AuroraLicenseSelectT", variant, 1, 0, false);
            else button.Load(folderName, "AuroraLicenseSelectB", variant, 1, 0, false);

        }
        kmCall(0x805eacf0, LoadNewLicenseButton);

        void SetLicenseClassColors(LicenseButton& button, u32 licenseClass){
                char buffer[0x10];
                snprintf(buffer, 0x10, "license_%d.tpl", licenseClass);
                if(licenseClass != 6)
                    CosmosUI::ChangePaneImage((LayoutUIControl*)&button, "license_plate", buffer);

                Pane* rootPane = button.layout.layout.rootPane;
                Pane* colorful = rootPane->FindPaneByName("colorful_base", true);

                if(colorful != nullptr){
                    colorful->GetMaterial()->tevKColours[0] = licenseColors[licenseClass][0];
                }

                SetPaneVTXColors(rootPane, "mii_base", licenseColors[licenseClass][1], 0x000000FF, 0x000000FF, licenseColors[licenseClass][1]);
                SetPaneVTXColors(rootPane, "player_base", licenseColors[licenseClass][1], licenseColors[licenseClass][2],
                        licenseColors[licenseClass][1], licenseColors[licenseClass][2]);
                SetPaneVTXColors(rootPane, "number_base", licenseColors[licenseClass][1], licenseColors[licenseClass][2],
                        licenseColors[licenseClass][1], licenseColors[licenseClass][2]);
                SetPaneVTXColors(rootPane, "license_base", licenseColors[licenseClass][1], licenseColors[licenseClass][2],
                        licenseColors[licenseClass][1], licenseColors[licenseClass][2]);

                SetTextBoxColors(rootPane, "l_number", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "player", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "GP", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "VR2", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "new", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);

                SetTextBoxColors(rootPane, "GP_STASS", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
                SetTextBoxColors(rootPane, "VR_STATS", licenseColors[licenseClass][3], licenseColors[licenseClass][4]);
        }

        //805eae94
        void SetupLicenseButton(u32 unknown, LicenseButton& button, int licenseIndex, MiiGroup* group, int buttonIndex){
            button.SetMiiPane("mii", group, buttonIndex, 0);
            RKPD& curLicense = SaveDataManager::GetStaticInstance()->rksysRaw->licenses[licenseIndex];
            button.SetTextBoxMsg("new", 0x1781, nullptr);

            if(curLicense.magic.raw != 0x524b5044){
                button.SetPaneVisible("new", true);
                button.SetPaneVisible("mii", false);
                button.SetPaneVisible("license_plate", false);

                button.ResetTextBoxMsg("player");
                SetLicenseClassColors(button, 6);
            }
            else {
                SetLicenseClassColors(button, Cosmos::Data::SettingsHolder::GetStaticInstance()->GetMaxLicenseClass(licenseIndex));
                button.SetPaneVisible("new", false);
                button.SetPaneVisible("mii", true);

                TextInfo miiInfo;
                miiInfo.miis[0] = group->GetMii(licenseIndex);
                button.SetTextBoxMsg("player", 0x251d, &miiInfo);

                TextInfo friendCodeInfo;
                TextInfo vrStats;
                TextInfo gpStats;

                LicenseManager& currentLicense = SaveDataManager::GetStaticInstance()->GetLicense(licenseIndex);
                u64 val = DWC_CreateFriendKey(&currentLicense.pid);
                if((val >> 32) != 0){
                    friendCodeInfo.intToPass[2] = val % 10000;
                    friendCodeInfo.intToPass[1] = (val / 10000) % 10000;
                    friendCodeInfo.intToPass[0] = (val / 100000000) % 10000;
                    button.SetTextBoxMsg("l_number", 0x83e, &friendCodeInfo);

                    vrStats.intToPass[0] = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetUserVR(licenseIndex);
                    button.SetTextBoxMsg("VR_STATS", 0x280B, &vrStats);

                }
                else {
                    button.SetTextBoxMsg("VR_STATS", 0x280C, nullptr);
                    button.SetTextBoxMsg("l_number", 0, nullptr);
                }
                button.SetTextBoxMsg("GP", 0x2807, nullptr);
                button.SetTextBoxMsg("VR2", 0x2808, nullptr);

                u32 trophyPoints = 0;
                for(int j = 0; j < 4; j++){
                    for(int i = 0; i < Cosmos::CupManager::GetStaticInstance()->GetMaxCupCount(); i++){
                        if(Cosmos::Data::SettingsHolder::GetStaticInstance()->IsGPCompleted(i, j, licenseIndex)){
                            trophyPoints += (3 - Cosmos::Data::SettingsHolder::GetStaticInstance()->GetGPTrophy(i, j, licenseIndex)); 
                        }
                    }
                }

                gpStats.intToPass[1] = Cosmos::CupManager::GetStaticInstance()->GetMaxCupCount() * 3 * 4;
                gpStats.intToPass[0] = trophyPoints;

                button.SetTextBoxMsg("GP_STASS", 0x280A, &gpStats);
            }
        }
        kmBranch(0x805eae94, SetupLicenseButton);
        kmBranch(0x805ec4e8, SetupLicenseButton);
/*
        void PatchLicenseButtonFile(PushButton& button, const char* folder, const char* filename, const char* variant, u32 localPlayerBitfield, u32 r8, bool inaccessible){
            button.Load(folder, "CosmosLicenseSelect", variant, localPlayerBitfield, r8, inaccessible);
        }
        */
        //kmBranch(0x805eae94, SetupLicenseButton);


        void LoadLicenseControl(ControlLoader& button, const char* folderName, const char* file, const char* variant, const char** a){
            button.Load(folderName, "AuroraLicenseManagement", variant, a);
        }
        kmCall(0x805ed948, LoadLicenseControl);
        
        void LoadLicenseMiiControl(ControlLoader& button, const char* folderName, const char* file, const char* variant, const char** a){
            button.Load(folderName, "AuroraLicenseDisplay", variant, a);
        }
        kmCall(0x805ec16c, LoadLicenseMiiControl);

        // TODO
        void PatchVRControlColor(Pages::VR& page, u32 index, u32 player, u32 team, u8 type, bool isLocal){
            page.FillVRControl(index, player, team, type, isLocal);

            Pane* pane = page.vrControls[index].layout.GetPaneByName("black_parts_t");

            RKNetUSERHandler* handler = RKNetUSERHandler::GetStaticInstance();
            u32 licenseClass = 0;

            Pages::CountDownTimer* timer = Pages::CountDownTimer::GetPage();
            if(isLocal){
                licenseClass = handler->toSendPacket.city;
            }
            else {
                u32 playerAid = timer->infos[player].aid;
                licenseClass = handler->receivedPackets[playerAid].city;
            }

            GXColorS10 color;
            color.r = (licenseColors[licenseClass][0] >> 24) & 0xFF;
            color.g = (licenseColors[licenseClass][0] >> 16) & 0xFF;
            color.b = (licenseColors[licenseClass][0] >> 8) & 0xFF;
            color.a = 0xFF;

            pane->GetMaterial()->tevColours[1] = color;
        }
        kmCall(0x8064aa78, PatchVRControlColor);

        void SendOnlineLicenseViaCity(RKNetUSERHandler& handler) {
            handler.toSendPacket.city = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetOnlineClass(); 
        };
        kmBranch(0x80662dc0, SendOnlineLicenseViaCity);

    }
}
