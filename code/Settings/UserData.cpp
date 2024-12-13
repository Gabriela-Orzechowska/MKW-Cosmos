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

#include "kamek.hpp"
#include <Settings/UserData.hpp>
#include <game/System/SaveDataManager.hpp>
#include <game/UI/Page/Other/WFCMain.hpp>
#include <game/Network/RKNetUser.hpp>
#include <UI/Language/LanguageManager.hpp>
#include <SlotExpansion/CupManager.hpp>

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
                    + sizeof(UserDataSettings) + sizeof(UserDataOther) + sizeof(UserDataTrophies)
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
            UserDataOther* otherFile;
            UserDataTrophies* trophiesFile;

            if (strcmp(mainFile->sign, magic) != 0 || mainFile->version != version)
            {
                settingsFile = (UserDataSettings*)(fileBuffer+sizeof(UserDataFile));
                otherFile = (UserDataOther*)(fileBuffer+sizeof(UserDataFile)+sizeof(UserDataSettings));
                trophiesFile = (UserDataTrophies*)(fileBuffer+sizeof(UserDataFile)+sizeof(UserDataSettings)+sizeof(UserDataOther));

                memset(mainFile, 0, bufferSize);
                strncpy(mainFile->sign, magic, 4);
                strncpy(settingsFile->sign, "ARSP", 4);
                strncpy(otherFile->sign, "AROP", 4);
                strncpy(trophiesFile->sign, "ARTP", 4);

                mainFile->version = version;
                settingsFile->version = version;
                otherFile->version = 1;
                trophiesFile->version = 1;

                mainFile->offsetToSettings = (u32)((u32)settingsFile - (u32)fileBuffer);
                mainFile->offsetToOthers = (u32)((u32)otherFile - (u32)fileBuffer);
                mainFile->offsetToThophies = (u32)((u32)trophiesFile - (u32)fileBuffer);

                for (int i = 0; i < PAGE_COUNT; i++)
                {
                    for (int j = 0; j < GlobalSettingDefinitions[i].settingCount; j++)
                    {
                        for(int o = 0; o < 4; o++)
                            settingsFile->data[o].pages[i].setting[j] = GlobalSettingDefinitions[i].settings[j].defaultValue;
                    }
                }
                for (int i = 0; i < (CupManager::GetStaticInstance()->GetCupCount()*4); i++){
                    trophiesFile->cups[i].gpData[0] = 0xFF;
                    trophiesFile->cups[i].gpData[1] = 0xFF;
                    trophiesFile->cups[i].gpData[2] = 0xFF;
                    trophiesFile->cups[i].gpData[3] = 0xFF;
                }

                for(int i = 0; i < 4; i++){
                    otherFile->vr[i] = 5000;
                    otherFile->br[i] = 5000;
                }
            }
            else {
                settingsFile = (UserDataSettings*) offsetFrom(mainFile, mainFile->offsetToSettings);
                otherFile = (UserDataOther*) offsetFrom(mainFile, mainFile->offsetToOthers);
                trophiesFile = (UserDataTrophies*) offsetFrom(mainFile, mainFile->offsetToThophies);
            }

            this->file = mainFile;
            this->settingsNew = settingsFile;
            this->other = otherFile;
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
            SettingsHolder::GetStaticInstance()->SetCurrentLicense(SaveDataManager::GetStaticInstance()->curLicenseId);
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
