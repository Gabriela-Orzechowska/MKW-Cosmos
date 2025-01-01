/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
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

#include "Input/inputdata.hpp"
#include "Race/Kart/KartParams.hpp"
#include "Race/RaceData.hpp"
#include "Race/RaceInfo.hpp"
#include "System/Identifiers.hpp"
#include "UI/MenuData/MenuData.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include <Settings/UserData.hpp>
#include <game/System/SaveDataManager.hpp>
#include <game/UI/Page/Other/WFCMain.hpp>
#include <game/Network/RKNetUser.hpp>
#include <UI/Language/LanguageManager.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <Race/Kart/KartHolder.hpp>

SettingsUpdateHook *SettingsUpdateHook::sHooks = NULL;
SettingsValueUpdateHook *SettingsValueUpdateHook::sHooks = NULL;
namespace Cosmos
{
    namespace Data
    {
        SettingsHolder *SettingsHolder::sInstance = NULL;

        SettingsHolder::SettingsHolder() : miiHeadsEnabled(true), currentLicense(0), megaCloudOnline(false), megaCloudOffline(false) {}

        void SettingsHolder::Update()
        {
            this->RequestSave();
            SettingsUpdateHook::exec();
        }

        void SettingsHolder::Init(const char *filepath, const char *magic, u32 version)
        {
            strncpy(this->filepath, filepath, IPCMAXPATH);

            u32 bufferSize = sizeof(UserDataFile) 
                    + sizeof(UserDataSettings) + sizeof(UserDataLicenses) + sizeof(UserDataTrophies)
                    + (CupManager::GetStaticInstance()->GetCupCount()*sizeof(UserDataCup)*4); // 4 per license
            bufferSize = (bufferSize + 0x1F) & ~0x1F;
            u8* fileBuffer = (u8*) RKSystem::mInstance.EGGSystem->alloc(bufferSize, 0x20);
            this->fileSize = bufferSize;

            CosmosFile::FileManager *manager = CosmosFile::FileManager::GetNANDManager();

            if (manager == nullptr)
            {
                CosmosError("Failed to get manager!!!\n");
                return;
            }
            currentManager = manager;

            manager->CreateOpen(this->filepath, CosmosFile::FILE_MODE_READ_WRITE);
            manager->Read(fileBuffer, bufferSize);

            UserDataFile* mainFile = (UserDataFile*) fileBuffer;

            UserDataSettings* settingsFile;
            UserDataLicenses* licensesFile;
            UserDataTrophies* trophiesFile;

            bool isValid = true;
            bool isValidTrophy = true;
            bool isValidSettings = true;
            bool isValidLicense = true;

            if(strcmp(mainFile->sign, USER_DATA_MAGIC) != 0 || mainFile->version != USER_DATA_VERSION) {
                isValid = false;
                isValidSettings = false;
                isValidTrophy = false;
                isValidLicense = false;

                settingsFile = (UserDataSettings*)(fileBuffer+sizeof(UserDataFile));
                licensesFile = (UserDataLicenses*)(fileBuffer+sizeof(UserDataFile)+sizeof(UserDataSettings));
                trophiesFile = (UserDataTrophies*)(fileBuffer+sizeof(UserDataFile)+sizeof(UserDataSettings)+sizeof(UserDataLicenses));
            }
            else {
                settingsFile = (UserDataSettings*) offsetFrom(mainFile, mainFile->offsetToSettings);
                licensesFile = (UserDataLicenses*) offsetFrom(mainFile, mainFile->offsetToOthers);
                trophiesFile = (UserDataTrophies*) offsetFrom(mainFile, mainFile->offsetToThophies);

                if(strcmp(settingsFile->sign, USER_DATA_SETTINGS_MAGIC) || settingsFile->version != USER_DATA_SETTINGS_VERSION)
                    isValidSettings = false;
                if(strcmp(trophiesFile->sign, USER_DATA_TROPHY_MAGIC) || trophiesFile->version != USER_DATA_TROPHY_VERSION)
                    isValidTrophy = false;
                if(strcmp(licensesFile->sign, USER_DATA_LICENSE_MAGIC) || licensesFile->version != USER_DATA_LICENSE_VERSION)
                    isValidTrophy = false;
            }

            if (!isValid)
            {
                memset(mainFile, 0, bufferSize);

                strncpy(mainFile->sign, USER_DATA_MAGIC, 4);
                mainFile->version = USER_DATA_VERSION;

                mainFile->offsetToSettings = (u32)((u32)settingsFile - (u32)fileBuffer);
                mainFile->offsetToOthers = (u32)((u32)licensesFile - (u32)fileBuffer);
                mainFile->offsetToThophies = (u32)((u32)trophiesFile - (u32)fileBuffer);
            }

            if(!isValidSettings) {
                strncpy(settingsFile->sign, USER_DATA_SETTINGS_MAGIC, 4);
                settingsFile->version = USER_DATA_SETTINGS_VERSION;

                for (int i = 0; i < PAGE_COUNT; i++)
                {
                    for (int j = 0; j < GlobalSettingDefinitions[i].settingCount; j++)
                    {
                        for(int o = 0; o < 4; o++)
                            settingsFile->data[o].pages[i].setting[j] = GlobalSettingDefinitions[i].settings[j].defaultValue;
                    }
                }
            }

            if(!isValidLicense) {
                strncpy(licensesFile->sign, USER_DATA_LICENSE_MAGIC, 4);
                licensesFile->version = USER_DATA_LICENSE_VERSION;

                for(int i = 0; i < 4; i++){
                    licensesFile->data[i].vr = 7500;
                    licensesFile->data[i].br = 7500;
                    licensesFile->data[i].onlineRaces = 0;
                    licensesFile->data[i].onlineScore = 400;
                }
            }

            if(!isValidTrophy) {
                strncpy(trophiesFile->sign, USER_DATA_TROPHY_MAGIC, 4);

                trophiesFile->version = USER_DATA_TROPHY_VERSION;

                for (int i = 0; i < (CupManager::GetStaticInstance()->GetMaxCupCount()*4*4); i++){
                    trophiesFile->cups[i>>2].gpData[i & 3] = 0xFF;
                }

            }

            this->file = mainFile;
            this->settingsNew = settingsFile;
            this->licenses = licensesFile;
            this->trophies = trophiesFile;
            manager->Overwrite(bufferSize, this->file);
            manager->Close();
        }

        void SettingsHolder::RequestSave()
        {
            CosmosFile::FileManager::GetStaticInstance()->taskThread->Request(&SettingsHolder::SaveTask, NULL, 0);
        }

        void SettingsHolder::Save()
        {
            currentManager->Open(this->filepath, CosmosFile::FILE_MODE_WRITE);
            currentManager->Overwrite(this->fileSize, this->file);
            currentManager->Close();
        }

        void SettingsHolder::SaveTask(void *)
        {
            SettingsHolder::GetStaticInstance()->Save();
        }

        void SettingsHolder::Create()
        {
            SettingsHolder *holder = new (RKSystem::mInstance.EGGSystem) SettingsHolder();
            char path[IPCMAXPATH];
            Cosmos::System::Console_Print("[CSE] Loading user settings\n");
            holder->Init(Cosmos::SaveFile, "ARSE", SettingsVersion);
            SettingsHolder::sInstance = holder;
        }

        void SettingsHolder::UpdateOnlineScore(RaceinfoPlayer& player){
            u32 license = this->currentLicense;

            u32 controllerId = RaceData::GetStaticInstance()->racesScenario.GetPlayer(player.id).realControllerId;

            if(controllerId < 0) {
                CosmosError("Attempted to get non local player score!\n");
                return;
            }

            Kart* kart = KartHolder::GetStaticInstance()->GetKart(player.id);
            GpStats* stats = kart->base.pointers->values->gpStats;

            if(stats == nullptr){
                CosmosError("GPStats were not found!\n");
                return;
            };

            u32 lowest = RaceInfo::GetStaticInstance()->GetPlayer(
                    RaceInfo::GetStaticInstance()->playerIdInEachPosition[0])->frameCounter;
            lowest = (lowest * 3) / 2;

            // No KRT, using computed value from leader
            s32 score = (1000 * (lowest - player.frameCounter) / lowest) + (150 * player.framesInFirst / lowest);

            //These values are modified, put original for reference

            // Bonuses
            if(stats->startBoostSuccessful) score += 25;
            score += stats->mtCount * 2;
            score += kart->base.pointers->values->raceStats->hitOtherCount * 5;

            Controller* controller = InputData::GetStaticInstance()->GetController(controllerId).controller;

            if(controller->GetType() == CONTROLLER_WII_WHEEL) score += 10;
            if(controller->isDriftAuto) score += 25;

            // Penalties
            score -= stats->offroadFrames / 3; // / 3
            score -= stats->wallHits * 20; // 20
            score -= stats->objectCollisionCount * 30; // 30
            score -= stats->oobCount * 30; // 70
                                           //
            CosmosLog("Last Race Stats:\nlowest: %d\nframeCounter: %d\nframesInFirst: %d\nscore: %d\n", lowest / 2, player.frameCounter, player.framesInFirst, score);

            if(score < -50) score = -50;
            else if(score > 250) score = 250;

            const u32 numVals = 6;
            score *= 4;

            CosmosLog("Final score: %d\n", score);

            this->licenses->data[license].onlineScore = ((numVals - 1) * this->licenses->data[license].onlineScore + score) / numVals;
            this->licenses->data[license].onlineRaces++;

            this->LicenseClassUpdate(license);
        };

        kmWrite32(0x80593058, 0x60000000); // Always create GPStats

        void SettingsHolder::LicenseClassUpdate(u32 license) {
            u32 currentClass = GetMaxLicenseClass(license);
            if(currentClass == 5) {
                this->Update();
                return;
            }

            u32 completedGps = 0;
            u32 averageGPRank = 0;

            u32 onlineRaces = this->licenses->data[license].onlineRaces;
            u32 onlineScore = this->licenses->data[license].onlineScore;

            u32 cupCount = Cosmos::CupManager::GetStaticInstance()->GetMaxCupCount();
            // We dont count 100cc
            for(int j = 1; j < 4; j++){
                for(int i = 0; i < cupCount; i++){
                    if(IsGPCompleted(i, j, license)){
                        completedGps++;
                        averageGPRank += GetGPRank(i, j, license);
                    }
                }
            }
            averageGPRank /= completedGps;

            LicenseClassRequirements& reqs = globalClassRequirements[currentClass];

            if (completedGps >= (((cupCount * reqs.gpCompletedCups) * 3) / 100) && averageGPRank <= reqs.gpAverageRank) {
                this->licenses->data[license].unlockFlags |= (1 << (currentClass));
            }
        
            else if ((onlineRaces >= reqs.onlineCompletedRaces && onlineScore >= reqs.onlineMinScore)) {
                this->licenses->data[license].unlockFlags |= (1 << (UserDataLicense::UNLOCK_OFFSET_ONLINE_LICENSE + currentClass));
            }
            this->Update();

        };

        void UpdateOnlineScoreAfterRace(){
            if(!isOnline()) return;
            for(int i = 0; i < RaceData::GetStaticInstance()->racesScenario.GetPlayerCount(); i++){
                if(RaceData::GetStaticInstance()->racesScenario.GetPlayer(i).playerType == PLAYER_REAL_LOCAL){
                    SettingsHolder::GetStaticInstance()->UpdateOnlineScore(*RaceInfo::GetStaticInstance()->GetPlayer(i));
                    return;
                }
            }
        }
        kmBranch(0x8052ed14, UpdateOnlineScoreAfterRace);

        BootHook InitSettings(SettingsHolder::Create, LOW);

        void SetBRAndVR(LicenseManager &license, u32 licenseId)
        {

            license.vr.mPoints = SettingsHolder::GetStaticInstance()->GetUserVR(licenseId);
            license.br.mPoints = SettingsHolder::GetStaticInstance()->GetUserBR(licenseId);
        }

        kmWrite32(0x80544f7c, 0x7fe3fb78);
        kmWrite32(0x80544f80, 0x7fc4f378);
        kmWrite32(0x80544f88, 0x48000028);
        kmCall(0x80544f84, SetBRAndVR);

        Rating *SaveVR(LicenseManager &license)
        {
            u16 curLicenseId = SaveDataManager::GetStaticInstance()->curLicenseId;
            LicenseManager &actualLicense = SaveDataManager::GetStaticInstance()->GetCurrentLicense();

            SettingsHolder *holder = SettingsHolder::GetStaticInstance();
            if (holder == nullptr)
                return 0;

            holder->SetUserVR(actualLicense.vr.mPoints, curLicenseId);
            holder->SetUserBR(actualLicense.br.mPoints, curLicenseId);

            holder->Save();
            return 0;
        }

        kmCall(0x80546998, SaveVR);

        // Skip saving vr to saveFile
        kmWrite32(0x805469a8, 0x60000000);
        kmWrite32(0x805469c0, 0x60000000);

        void LoadLicenseSettings(){
            CosmosLog("CurrentLicense: %d\n", MenuData::GetStaticInstance()->GetCurrentContext()->licenseId);
            SettingsHolder::GetStaticInstance()->SetCurrentLicense(MenuData::GetStaticInstance()->GetCurrentContext()->licenseNum);
            SettingsUpdateHook::exec();
            if(LanguageManager::GetStaticInstance()->IsUpdateNeeded()) Page::transitionDelay = 176.0f;
            else {
                float delay = 176.0f;
                if(SettingsHolder::GetStaticInstance()->GetSettingValue(COSMOS_SETTING_FAST_MENUS) == ENABLED) 
                    delay = 0.0f;
                Page::transitionDelay = delay;
            }
        }
        kmBranch(0x805ebb40, LoadLicenseSettings);
    }
} // namespace Cosmos::Data
