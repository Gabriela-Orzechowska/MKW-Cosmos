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

#include <Settings/UserData.hpp>
#include <game/System/SaveDataManager.hpp>
#include <game/UI/Page/Other/WFCMain.hpp>
#include <game/Network/RKNetUser.hpp>
#include <UI/Language/LanguageManager.hpp>

SettingsUpdateHook *SettingsUpdateHook::sHooks = NULL;
SettingsValueUpdateHook *SettingsValueUpdateHook::sHooks = NULL;
namespace Cosmos
{
    namespace Data
    {
        SettingsHolder *SettingsHolder::sInstance = NULL;

        SettingsHolder::SettingsHolder() : miiHeadsEnabled(true), currentLicense(0), megaCloudOnline(false), megaCloudOffline(false) { settings = NULL; }

        void SettingsHolder::Update()
        {
            this->RequestSave();
            SettingsUpdateHook::exec();
        }

        void SettingsHolder::Init(const char *filepath, const char *magic, u32 version)
        {
            strncpy(this->filepath, filepath, IPCMAXPATH);

            Settings *buffer = new (RKSystem::mInstance.EGGSystem, 0x20) Settings;

            CosmosFile::FileManager *manager = CosmosFile::FileManager::GetNANDManager();

            if (manager == nullptr)
            {
                CosmosError("Failed to get manager!!!\n");
                return;
            }

            manager->CreateOpen(this->filepath, CosmosFile::FILE_MODE_READ_WRITE);
            manager->Read(buffer, sizeof(Settings));
            if (strcmp(buffer->signature, magic) != 0 || buffer->version != version)
            {
                memset(buffer, 0, sizeof(Settings));
                strncpy(buffer->signature, magic, 4);
                buffer->version = version;
                for (int i = 0; i < 4; i++)
                {
                    buffer->playerVr[i] = 5000;
                    buffer->playerBr[i] = 5000;
                }

                for (int i = 0; i < PAGE_COUNT; i++)
                {
                    for (int j = 0; j < GlobalSettingDefinitions[i].settingCount; j++)
                    {
                        for(int o = 0; o < 4; o++)
                            buffer->data[o].pages[i].setting[j] = GlobalSettingDefinitions[i].settings[j].defaultValue;
                    }
                }
            }
            this->settings = buffer;
            manager->Overwrite(sizeof(Settings), buffer);
            manager->Close();
        }

        void SettingsHolder::RequestSave()
        {
            CosmosFile::FileManager::GetStaticInstance()->taskThread->Request(&SettingsHolder::SaveTask, NULL, 0);
        }

        void SettingsHolder::Save()
        {
            CosmosFile::FileManager *manager = CosmosFile::FileManager::GetNANDManager();
            manager->Open(this->filepath, CosmosFile::FILE_MODE_WRITE);
            manager->Overwrite(sizeof(Settings), this->settings);
            manager->Close();
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
            holder->Init(Cosmos::SaveFile, "CSSE", SettingsVersion);
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
