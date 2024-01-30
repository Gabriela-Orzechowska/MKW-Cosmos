#include <Settings/UserData.hpp>

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
        holder->Init(path, "CSSD", 0x01);
        SettingsHolder::sInstance = holder;
    }

    BootHook InitSettings(SettingsHolder::Create, MEDIUM);

} // namespace CosmosData
