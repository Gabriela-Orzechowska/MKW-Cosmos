#include "kamek.hpp"
#include <UI/Language/LanguageManager.hpp>
#include <game/Scene/RootScene.hpp>
#include <main.hpp>

namespace Cosmos
{
    LanguageManager* LanguageManager::sInstance = nullptr;

    void LanguageManager::CreateStaticInstance(){
        if(sInstance == nullptr) 
            sInstance = new (RKSystem::mInstance.EGGSystem) LanguageManager;
        sInstance->Init();
    } 

    u32 LanguageManager::GetActualLanguage() {
        if(this->currentLanguageOption == DEFAULT){
            switch(gameID[3]){
                case 'P':
                    return GameToGlobal(this->systemLanguage, 0);
                case 'E':
                    return GameToGlobal(this->systemLanguage, 1);
                case 'J':
                    return JAPANESE;
                case 'K':
                    return KOREAN;
                default:
                    return DEFAULT;
            }
        }
        else return this->currentLanguageOption;
    }

    u32 LanguageManager::GameToGlobal(u32 i, u32 a){
        switch(i){
            case LanguageManager::P_ENGLISH:
                return LanguageManager::ENGLISH + a;
            case LanguageManager::P_GERMAN:
                return LanguageManager::GERMAN;
            case LanguageManager::P_FRENCH:
                return LanguageManager::FRENCH + a;
            case LanguageManager::P_SPANISH:
                return LanguageManager::SPANISH + a;
            case LanguageManager::P_ITALIAN:
                return LanguageManager::ITALIAN;
            case LanguageManager::P_DUTCH:
                return LanguageManager::DUTCH;
            default:
                return LanguageManager::DEFAULT;
        }
    }

    void LanguageManager::Init() {
        this->systemLanguage = SystemManager::GetStaticInstance()->strapPageLanguage;
        this->needsUpdate = false;
        this->isBoot = true;        
        Update(false);
    }

    bool LanguageManager::IsUpdateNeeded(){
        return this->needsUpdate;
    }

    void ResetLanguageManagerStatus(){
        LanguageManager::GetStaticInstance()->ResetStatus();
    }
    static MenuLoadHook mlhResetLanguageManagerStatus(ResetLanguageManagerStatus);

    void LanguageManager::Update(bool reload) {
        this->currentLanguageOption = this->isBoot ? DEFAULT : Cosmos::Data::SettingsHolder::GetStaticInstance()->GetSettingValue(Cosmos::Data::COSMOS_SETTING_LANGUAGE_SETTINGS);
        this->isDefault = this->isBoot ? true : this->currentLanguageOption == DEFAULT;
        //this->needsUpdate = this->isKorean != (this->currentLanguageOption == KOREAN);
        this->isKorean = this->currentLanguageOption == KOREAN;

        this->actualLanguage = this->GetActualLanguage();
        this->needsUpdate = this->actualLanguage != this->lastLanguage;
        
        char buffer[0x80];
        snprintf(buffer, 0x80, "%s.szs", suffixes[this->actualLanguage]);

        char superMenuName[0x80];
        snprintf(superMenuName, 0x80, "/Scene/UI/SuperMenu%s.szs", suffixes[this->actualLanguage]);

        strncpy(ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_COMMON)->archiveSuffixes[0x1], buffer, 0x80);
        strncpy(ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_UI)->archiveSuffixes[0x1], superMenuName, 0x80);
        ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_UI)->sourceType[0x1] = ArchivesHolder::FULL_FILE_PATH;
        this->isBoot = false;
        if(reload){
            if(this->actualLanguage != this->lastLanguage){
                if(Scene::GetType(MenuData::GetStaticInstance()->curScene->menuId) != CATEGORY_GAMEPLAY){
                    CosmosUI::NewSettings* page = MenuData::GetStaticInstance()->curScene->Get<CosmosUI::NewSettings>((PageId)Cosmos::SETTINGS_MAIN);
                    if(page != nullptr) page->ChangeMenu(page->GetPreviousMenu(), 0, 0.0f);
                    else {
                        Page* page = MenuData::GetStaticInstance()->curScene->Get<Page>(MAIN_MENU_PAGE);
                        if(page != nullptr) page->ChangeMenu(MAIN_MENU_FROM_MENU, 0, 0.0f);
                    }
                }
            }
        }
        this->lastLanguage = this->actualLanguage;
    }

    kmWrite32(0x8000ad9c, 0x38000006); //System Dutch

    void InitManager() {
        LanguageManager::CreateStaticInstance();
    }
    kmBranch(0x8053fc98, InitManager);

    void UpdateLanguage(){
        LanguageManager::GetStaticInstance()->Update(true);
    }
    static SettingsUpdateHook UpdateSystemLanguage(UpdateLanguage);

    kmWrite32(0x800b28f8,0x4800000c);

} // namespace Cosmos
