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

#include "FileManager/FileManager.hpp"
#include "Input/inputdata.hpp"
#include "Race/Kart/KartParams.hpp"
#include "Race/RaceData.hpp"
#include "Race/RaceInfo.hpp"
#include "System/Identifiers.hpp"
#include "UI/MenuData/MenuData.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include "System/System.hpp"
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

        u32 SettingsHolder::GetRevisionPageCount(u32 revision){
            (void)revision;
            return 7;
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
            memset(fileBuffer, 0, bufferSize);

            CosmosFile::FileManager *manager = CosmosFile::FileManager::GetNANDManager();

            if (manager == nullptr)
            {
                CosmosError("Failed to get manager!!!\n");
                return;
            }
            currentManager = manager;
            manager->CreateOpen(this->filepath, CosmosFile::FILE_MODE_READ_WRITE);
            u32 bytesRead = manager->Read(fileBuffer, bufferSize);

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

                if(strcmp(settingsFile->sign, USER_DATA_SETTINGS_MAGIC) || settingsFile->version > USER_DATA_SETTINGS_VERSION)
                    isValidSettings = false;
                if(strcmp(trophiesFile->sign, USER_DATA_TROPHY_MAGIC) || trophiesFile->version > USER_DATA_TROPHY_VERSION)
                    isValidTrophy = false;
                if(strcmp(licensesFile->sign, USER_DATA_LICENSE_MAGIC) || licensesFile->version > USER_DATA_LICENSE_VERSION)
                    isValidTrophy = false;
            }

            if(isValid && settingsFile->version < USER_DATA_SETTINGS_VERSION){
                void* settingsFileP = (void*)(fileBuffer+sizeof(UserDataFile));
                void* licensesDataP = (void*)(fileBuffer+sizeof(UserDataFile)+sizeof(UserDataSettings));
                void* trophyDataP = (void*)(fileBuffer+sizeof(UserDataFile)+sizeof(UserDataSettings)+sizeof(UserDataLicenses));

                u32 trophyBufferSize = sizeof(UserDataTrophies)
                        + (CupManager::GetStaticInstance()->GetCupCount()*sizeof(UserDataCup)*4); // 4 per license
                trophyBufferSize = (trophyBufferSize + 0x1F) & ~0x1F;
                u8* tempBufferTr = (u8*) RKSystem::mInstance.EGGSystem->alloc(trophyBufferSize, 0x20);

                u8 tempBufferLc[sizeof(UserDataLicenses)];

                u32 oldVersion = settingsFile->version;

                u8 rawSettingsBuff[4][(PAGE_COUNT+1)* SETTINGS_PER_PAGE];

                u8 oldSettingsSize = SETTINGS_PER_PAGE * (GetRevisionPageCount(oldVersion) + 1);
                for(int i = 0; i < 4; i++){
                    memcpy(rawSettingsBuff[i], ((u8*)&settingsFile->data[0]) + (i * oldSettingsSize), oldSettingsSize);
                }

                memcpy(tempBufferTr, trophiesFile, trophyBufferSize);
                memcpy(tempBufferLc, licensesFile, sizeof(UserDataLicenses));

                memset(trophyDataP, 0, trophyBufferSize);
                memset(licensesDataP, 0, sizeof(UserDataLicenses));

                memcpy(trophyDataP, tempBufferTr, trophyBufferSize);
                memcpy(licensesDataP, tempBufferLc, sizeof(UserDataLicenses));

                RKSystem::mInstance.EGGSystem->free(tempBufferTr);

                memset(settingsFile, 0, sizeof(UserDataSettings));

                strncpy(settingsFile->sign, USER_DATA_SETTINGS_MAGIC, 4);
                settingsFile->version = USER_DATA_SETTINGS_VERSION;

                for(int i = 0; i < 4; i++){
                    memcpy(&settingsFile->data[i], rawSettingsBuff[i], oldSettingsSize);
                }

                CosmosLog("Updating settings, old rev: %d, new rev: %d\n", oldVersion, USER_DATA_SETTINGS_VERSION);

                for (int i = 0; i < PAGE_COUNT+1; i++)
                {
                    for (int j = 0; j < GlobalSettingDefinitions[i].settingCount; j++)
                    {
                        for(int o = 0; o < 4; o++) {
                            if(GlobalSettingDefinitions[i].settings[j].revisionAdded >= oldVersion) {
                                settingsFile->data[o].pages[i].setting[j] = GlobalSettingDefinitions[i].settings[j].defaultValue;
                            }
                        }
                    }
                }
                isValidSettings = true;
                trophiesFile = (UserDataTrophies*)trophyDataP;
                licensesFile = (UserDataLicenses*)licensesDataP;
                mainFile->offsetToOthers = (u32)((u32)licensesFile - (u32)fileBuffer);
                mainFile->offsetToThophies = (u32)((u32)trophiesFile - (u32)fileBuffer);
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

                for (int i = 0; i < PAGE_COUNT+1; i++)
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
                    licensesFile->data[i].vr = 6250;
                    licensesFile->data[i].br = 6250;
                    licensesFile->data[i].onlineRaces = 0;
                    licensesFile->data[i].onlineScore = 400;
                    licensesFile->data[i].unlockFlags = 0;
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

            int playerCount = RaceData::GetStaticInstance()->racesScenario.playerCount;
            if(playerCount < 3) {
                this->licenses->data[license].onlineRaces++;
                this->LicenseClassUpdate(license);
                return;
            }

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
            lowest = (lowest * 5) / 4;

            // No KRT, using computed value from leader
            s32 score = (1000 * (lowest - player.frameCounter) / lowest) + (150 * player.framesInFirst / lowest);

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
            score -= stats->oobCount * 70; // 70

            if(score < -50) score = -50;
            else if(score > 250) score = 250;

            int scoreWeight = (playerCount - 2);
            if(scoreWeight > 6) scoreWeight = 6;

            const u32 numVals = 36;
            score *= 4;

            this->licenses->data[license].onlineScore = ((numVals - scoreWeight) 
                    * this->licenses->data[license].onlineScore + (scoreWeight * score)) / numVals;
            this->licenses->data[license].onlineRaces++;

            this->LicenseClassUpdate(license);
        };

        kmWrite32(0x80593058, 0x60000000); // Always create GPStats

        void SettingsHolder::LicenseClassUpdate(u32 license) {
            if((this->licenses->data[license].unlockFlags & UserDataLicense::UNLOCK_LOCK) == UserDataLicense::UNLOCK_LOCK)
                return;

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
