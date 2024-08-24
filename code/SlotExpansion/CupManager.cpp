#include <System/Security.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <core/RK/RKSystem.hpp>
#include <FileManager/FileManager.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>
#include <game/Sound/RaceAudioManager.hpp>
#include <main.hpp>

extern u32 p_tracklist1;
extern u32 p_tracklist2;
extern u32 p_tracklist1_2;
extern u32 p_tracklist2_2;

const char ConfigPath[] = "/Binaries/Config.cscf";

namespace Cosmos
{
    CupManager * CupManager::sInstance = nullptr;

    CupManager::CupManager() : winningCourse(0), lastSelectedButton(0), lastSelectedCourse(0), lastSelectedCup(0), dontUpdateCourseSelectCourse(0),
    currentTrackBlockIndex(0)
    {
        if(CupManager::sInstance != nullptr) return;
        CupManager::sInstance = this;

        memset(this->trackBlocking, -1U, 0x10 * sizeof(u32));

        Cosmos::System::Console_Print("[CSE] Loading config file\n");

        DVDFileInfo fileHandle;
        if(!DVDOpen(ConfigPath, &fileHandle))
        {
            Cosmos::Panic(__FILE__, __LINE__, "Failed to open %s\n", ConfigPath);
        }

        CupConfig* config = (CupConfig*) new (RKSystem::mInstance.EGGSystem, 0x20) char[fileHandle.length];

        if(!DVDReadPrio(&fileHandle, (void*)config, fileHandle.length, 0x0, 0x2)){
            Cosmos::Panic(__FILE__, __LINE__, "Failed to read %s\n", ConfigPath);
        }

        if(config->fileVersion != 3) {
            Cosmos::System::Console_Print("[ERR] Invalid config version!\n");
            COSMOS_HANG();
        }

        this->cupConfig = config;
        CosmosLog("Cup Config at: %p\n", config);
        this->definitions = (Track*)offsetFrom(config, config->offToDefinitions);
        SetTrackLayout(DEFAULT);

        DVDClose(&fileHandle);
            
    }

    void CupManager::UpdateSelectedCourse(PushButton * button){
        this->lastSelectedCourse = button->buttonId;

        this->winningCourse = this->lastSelectedCourse;
        if(Cosmos::isGroupSlot(this->lastSelectedCourse)) this->winningCourse = this->GetRandomVariantTrack(this->lastSelectedCourse);
    }

    void CupManager::SetTrackLayout(TrackSorting layout, u32 trackList)
    {
        this->currentLayout = (LayoutDef*)offsetFrom(this->cupConfig, this->cupConfig->offToLayouts[trackList * 2 + layout]);
        this->currentLayoutArray = this->currentLayout->slots;

        CosmosLog("Tracklist Pointer: %p\n", this->currentLayoutArray); 
    }

    int CupManager::GetCurrentMusicSlot() const
    {
        if(isRTSlot(this->winningCourse)) return RaceAudioMgr::GetStaticInstance()->GetCourseSoundId();
        return RaceAudioMgr::GetStaticInstance()->trackToMusicIDTable[definitions[this->winningCourse - CT_OFFSET].musicSlot];
    }

    bool CupManager::IsInBlocking(int track) const{
        for(int i = 0; i < 0x10; i++){
            if(track == trackBlocking[i]) return true;
        }
        return false;
    }

    int CupManager::GetRandomTrack() const
    {
        Random rand;
        int trackIndex = -1;
        int currentTry = 0;
        do {
            trackIndex = rand.NextLimited(this->GetTrackCount());
            currentTry++;
        }
        while(IsInBlocking(currentLayoutArray[trackIndex]) && currentTry < 0x10);

        return currentLayoutArray[trackIndex];
    }

    int CupManager::GetRandomSlot() const {
        Random rand;
        return currentLayoutArray[rand.NextLimited(this->GetTrackCount())];
    }

    u32 CupManager::GetRandomVariantTrack(u32 slot) const{
        if(isGroupSlot(slot)) slot -= GROUP_OFFSET;
        else return -1U;
        Random rand;
        VariantDef* def = (VariantDef*) offsetFrom(this->cupConfig, this->cupConfig->offToVariants);
        return def[slot].slot[rand.NextLimited(def[slot].count)];
    }

    int CupManager::GetCurrentTrackSlot() const
    {
        if(isRTSlot(this->winningCourse)) return this->winningCourse;

        return definitions[this->winningCourse - CT_OFFSET].slot;
    }

    int PatchSlot(Pages::CourseSelect * page, CtrlMenuCourseSelectCourse * ctrl, PushButton * button)
    {
        CupManager * manager = CupManager::GetStaticInstance();
        manager->UpdateSelectedCourse(button);
        return manager->GetCurrentTrackSlot();
    }

    asm int PatchSlotWrapper(){
        ASM(
            nofralloc;
            mflr r31;
            bl PatchSlot;
            mtlr r31;
            mr r31, r3;
            blr;
        )
    }
    kmCall(0x80840858, PatchSlotWrapper);

    void InitCupDefinition()
    {
        new (RKSystem::mInstance.EGGSystem) CupManager();
    }

    static BootHook bhInitCupDef(InitCupDefinition, MEDIUM);


} // namespace Cosmos
