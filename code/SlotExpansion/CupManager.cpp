#include <SlotExpansion/CupManager.hpp>
#include <core/RK/RKSystem.hpp>
#include <FileManager/FileManager.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>

namespace Cosmos
{
    CupManager * CupManager::sInstance = nullptr;

    CupManager::CupManager()
    {
        if(CupManager::sInstance != nullptr) return;
        CupManager::sInstance = this;

        winningCourse = 0; lastSelectedCourse = 0; lastSelectedCup = 0; lastSelectedButton = 0;

        DVDFileInfo fileHandle;
        if(DVDOpen("/cup/def.bin", &fileHandle))
        {
            char buffer[0x20] __attribute__ ((aligned(0x20)));
            if(DVDReadPrio(&fileHandle, (void *) buffer, 0x20, 0x0, 0x2))
            {
                Cups * tmp = (Cups *) buffer;
                this->cupDef = (Cups *) new (RKSystem::mInstance.EGGSystem, 0x20) char[tmp->fileSize]; 

                if(DVDReadPrio(&fileHandle, (void *) this->cupDef, tmp->fileSize, 0x0, 0x2)){
                    CosmosLog("File loaded to: %p\n", this->cupDef);
                }
            }

            DVDClose(&fileHandle);

            if(DVDOpen("/cup/layout_def.bin", &fileHandle))
            {
                this->currentLayoutArray = new (RKSystem::mInstance.EGGSystem, 0x20) u32[this->cupDef->cupCount * 4];
                if(DVDReadPrio(&fileHandle, (void *) this->currentLayoutArray, sizeof(u32) * this->cupDef->cupCount * 4, 0x0, 0x2)){
                    CosmosLog("Layout loaded to: %p\n", this->currentLayoutArray);
                }
            }
        }
    }

    void CupManager::UpdateSelectedCourse(PushButton * button){
        this->lastSelectedCourse = button->buttonId;
        this->winningCourse = lastSelectedCourse;
    }

    int CupManager::GetCurrentTrackSlot()
    {
        return this->cupDef->tracks[this->winningCourse].slot;
    }

    int PatchSlot(Pages::CourseSelect * page, CtrlMenuCourseSelectCourse * ctrl, PushButton * button)
    {
        CupManager * manager = CupManager::sInstance;
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

    static BootHook bhInitCupDef(InitCupDefinition, FIRST);


} // namespace Cosmos
