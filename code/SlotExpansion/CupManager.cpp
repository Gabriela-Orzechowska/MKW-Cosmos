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

namespace Cosmos
{
    CupManager * CupManager::sInstance = nullptr;

    CupManager::CupManager() : winningCourse(0), lastSelectedButton(0), lastSelectedCourse(0), lastSelectedCup(0), dontUpdateCourseSelectCourse(0),
    currentTrackBlockIndex(0)
    {
        if(CupManager::sInstance != nullptr) return;
        CupManager::sInstance = this;

        memset(this->trackBlocking, ~0x0, 0x10 * sizeof(u32));

        DVDFileInfo fileHandle;
        if(!DVDOpen("/cup/config.cscf", &fileHandle))
        {
            Cosmos::Panic(__FILE__, __LINE__, "Failed to open /cup/config.cscf!\n");
        }
        char buffer[0x20] __attribute__ ((aligned(0x20)));  
        if(!DVDReadPrio(&fileHandle, (void *) buffer, 0x20, 0x0, 0x2))
        {
            Cosmos::Panic(__FILE__, __LINE__, "Failed to read /cup/config.cscf!\n");
        }
        CupConfig* tempConfig = (CupConfig*) buffer;

        CupConfig* config = (CupConfig*) new (RKSystem::mInstance.EGGSystem, 0x20) char[tempConfig->fileSize];

        if(!DVDReadPrio(&fileHandle, (void*)config, tempConfig->fileSize, 0x0, 0x2)){
            Cosmos::Panic(__FILE__, __LINE__, "Failed to read /cup/config.cscf!\n");
        }

        this->cupConfig = config;
        CosmosLog("Cup Config at: %p\n", config);
        this->definitions = (Track*)offsetFrom(config, config->offToDefinitions);
        this->layouts[0] = (u32*)offsetFrom(config, config->offToLayouts[0]);
        this->layouts[1] = (u32*)offsetFrom(config, config->offToLayouts[1]);
        SetTrackLayout(ALPHABETICAL);

        DVDClose(&fileHandle);
            
    }

    void CupManager::UpdateSelectedCourse(PushButton * button){
        this->lastSelectedCourse = button->buttonId;
        this->winningCourse = this->lastSelectedCourse;
    }

    void CupManager::SetTrackLayout(TrackLayout layout)
    {
        this->currentLayoutArray = this->layouts[layout];

        p_tracklist1 = 0x3ca00000 | (((u32)this->currentLayoutArray) >> 16);
        p_tracklist2 = 0x60a50000 | (((u32)this->currentLayoutArray) & 0x0000FFFF);

        p_tracklist1_2 = 0x3fc00000 | (((u32)this->currentLayoutArray) >> 16);
        p_tracklist2_2 = 0x63de0000 | (((u32)this->currentLayoutArray) & 0x0000FFFF);
    }

    int CupManager::GetCurrentMusicSlot()
    {
        if(this->winningCourse < CT_OFFSET) return RaceAudioMgr::GetStaticInstance()->GetCourseSoundId();
        return RaceAudioMgr::GetStaticInstance()->trackToMusicIDTable[definitions[this->winningCourse - CT_OFFSET].musicSlot];
    }

    int CupManager::GetRandomTrack() const
    {
        Random rand;
        int trackIndex = rand.NextLimited(this->GetTrackCount());
        // TODO Include track blocking
        return currentLayoutArray[trackIndex];
    }
    int CupManager::GetCurrentTrackSlot()
    {
        if(this->winningCourse < CT_OFFSET) return this->winningCourse;

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
