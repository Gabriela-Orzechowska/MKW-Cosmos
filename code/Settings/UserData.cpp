#include <Settings/UserData.hpp>
#include <core/rvl/os/OS.hpp>

SettingUpdateHook * SettingUpdateHook::sHooks = NULL;

namespace DXData
{
    SettingsHolder * SettingsHolder::sInstance = NULL;
    
    SettingsHolder::SettingsHolder() { settings = NULL; }

    SettingsHolder * SettingsHolder::GetInstance() { return SettingsHolder::sInstance; }

    void SettingsHolder::Update()
    {
        SettingUpdateHook::exec();
        this->RequestSave();
    }

    void SettingsHolder::Init(char * filepath, const char * magic, u32 version)
    {
        strncpy(this->filepath, filepath, IPCMAXPATH);
        Settings * buffer = new (RKSystem::mInstance.EGGSystem, 0x20) Settings;

        DXFile::FileManager * manager = DXFile::FileManager::GetStaticInstance();
        manager->CreateOpen(this->filepath, DXFile::FILE_MODE_READ_WRITE);
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
        DXFile::FileManager::GetStaticInstance()->taskThread->Request(&SettingsHolder::SaveTask, NULL, 0);
    }

    void SettingsHolder::Save(){
        DXFile::FileManager * manager = DXFile::FileManager::GetStaticInstance();
        manager->Open(this->filepath, DXFile::FILE_MODE_WRITE);
        manager->Overwrite(sizeof(Settings), this->settings);
        manager->Close();
    }

    void SettingsHolder::SaveTask(void*){
        SettingsHolder::GetInstance()->Save();
    }

    void SettingsHolder::Create(){
        SettingsHolder * holder = new (RKSystem::mInstance.EGGSystem) SettingsHolder();
        char path[IPCMAXPATH] = "/mkwdx/DXData.bin";
        holder->Init(path, "DXSD", 0x01);
        SettingsHolder::sInstance = holder;
        OSReport("[DX] Create mkwdx/DXData.bin");
    }

    BootHook InitSettings(SettingsHolder::Create, MEDIUM);

} // namespace DXData
