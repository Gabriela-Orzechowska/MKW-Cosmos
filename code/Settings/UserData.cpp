#include <Settings/UserData.hpp>
#include <game/System/SaveDataManager.hpp>
#include <game/UI/Page/Other/WFCMain.hpp>
#include <game/Network/RKNetUser.hpp>

SettingsUpdateHook * SettingsUpdateHook::sHooks = NULL;

namespace CosmosData
{
    SettingsHolder * SettingsHolder::sInstance = NULL;
    
    SettingsHolder::SettingsHolder() { settings = NULL; }

    SettingsHolder * SettingsHolder::GetInstance() { return SettingsHolder::sInstance; }

    void SettingsHolder::Update()
    {
        this->RequestSave();
        SettingsUpdateHook::exec();
    }

    void SettingsHolder::Init(char * filepath, const char * magic, u32 version)
    {
        strncpy(this->filepath, filepath, IPCMAXPATH);


        Settings * buffer = new (RKSystem::mInstance.EGGSystem, 0x20) Settings;

        CosmosFile::FileManager * manager = CosmosFile::FileManager::GetStaticInstance();
        
        if(manager == nullptr)
        {
            CosmosError("Failed to get manager!!!\n");
            return;
        }
        
        manager->CreateOpen(this->filepath, CosmosFile::FILE_MODE_READ_WRITE);
        manager->Read(buffer, sizeof(Settings));
        if(strcmp(buffer->signature, magic) != 0 || buffer->version != version)
        {
            memset(buffer, 0, sizeof(Settings));
            strncpy(buffer->signature, magic, 4);
            buffer->version = version;

            for(int i = 0; i < 4; i++){
                buffer->playerVr[i] = 5001;
                buffer->playerBr[i] = 2137;
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

    void SettingsHolder::Save(){
        CosmosFile::FileManager * manager = CosmosFile::FileManager::GetStaticInstance();
        manager->Open(this->filepath, CosmosFile::FILE_MODE_WRITE);
        manager->Overwrite(sizeof(Settings), this->settings);
        manager->Close();
    }

    void SettingsHolder::SaveTask(void*){
        SettingsHolder::GetInstance()->Save();
    }

    void SettingsHolder::Create(){
        SettingsHolder * holder = new (RKSystem::mInstance.EGGSystem) SettingsHolder();
        char path[IPCMAXPATH];
        snprintf(path, IPCMAXPATH, "%s/%s", Cosmos::packFolder, Cosmos::SaveFile);
        holder->Init(path, "CSSD", 0x02);
        SettingsHolder::sInstance = holder;
    }

    BootHook InitSettings(SettingsHolder::Create, MEDIUM);

    void SetBRAndVR(LicenseManager * license, u32 licenseId){
        license->vr.mPoints = SettingsHolder::GetInstance()->GetSettings()->playerVr[licenseId];
        license->br.mPoints = SettingsHolder::GetInstance()->GetSettings()->playerBr[licenseId];
    }

    kmWrite32(0x80544f7c, 0x7fe3fb78);
    kmWrite32(0x80544f80, 0x7fc4f378);
    kmWrite32(0x80544f88, 0x48000028);
    kmCall(0x80544f84, SetBRAndVR);

    u32 currentLicense = 0; // the only thing saving me is the fact save is always done in a for loop
    Rating * SaveVR(LicenseManager * license){

        u16 curLicenseId = SaveDataManager::sInstance->curLicenseId;
        LicenseManager * actualLicense = &SaveDataManager::sInstance->licenses[curLicenseId];

        SettingsHolder::GetInstance()->GetSettings()->playerVr[curLicenseId] = actualLicense->vr.mPoints;
        SettingsHolder::GetInstance()->GetSettings()->playerBr[curLicenseId] = actualLicense->br.mPoints;
        SettingsHolder::GetInstance()->Update();
        return &license->vr;
    }

    kmCall(0x80546998, SaveVR);

    //Skip saving vr to saveFile
    kmWrite32(0x805469a8, 0x60000000);
    kmWrite32(0x805469c0, 0x60000000);

} // namespace CosmosData
