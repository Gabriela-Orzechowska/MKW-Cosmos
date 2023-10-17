#include <UI/CupSelect/CupSelect.hpp>
#include <LeCode/LeCodeManager.hpp>

namespace DXUI
{
    CupSelectPlus * CreateCupPage()
    {
        return new(CupSelectPlus);
    };

    kmWrite32(0x80623d98, 0x60000000);
    kmCall(0x80623da4, CreateCupPage);

    CupSelectPlus::CupSelectPlus()
    {
        onSwitchPressHandler.subject = this;
        onSwitchPressHandler.ptmf = &CupSelectPlus::OnSwitchPress;
        this->controlsManipulatorManager.SetGlobalHandler(SWITCH_PRESS, (PtmfHolder_1A<Page, void, u32>*)&onSwitchPressHandler, false, false);
    }

    void CupSelectPlus::OnSwitchPress(u32 slotId)
    {        

        currentLayout = static_cast<CUP_LAYOUT>(((u32)currentLayout) ^ 0x1);

        DVDFileInfo fileHandle;
        char * filepath = "/bin/cup2.bin";

        LeCode::cup2_header * header = (LeCode::cup2_header * ) LeCode::LeCodeManager::GetStaticInstance()->GetCupParamAddress();

        char * address = (char *) LeCode::LeCodeManager::GetStaticInstance()->GetCupDefAddress();
        u32 extraOffset = 0x10;

        if((u32)address & 0x10)
        {
            address -= 0x10;
            extraOffset = 0x0;
        }

        if(DVDOpen(filepath, &fileHandle))
        {
            u32 offset = ((u32)currentLayout) * CUPFILE_SORT_OFFSET;

            offset += extraOffset;

            u32 num_bytes = 0x10 * (header->course_cups);
            num_bytes = ((num_bytes + 31) & ~31);
            //address = LeCode::LeCodeManager::GetStaticInstance()->GetCupDefAddress();

            DVDReadPrio(&fileHandle, address, num_bytes, offset, 0x2);
            DVDClose(&fileHandle);
        }

        if(((u32)currentLayout) & 0x1)
        {
            PageId lastPage = this->prevPageId;
            this->LoadNextPageWithDelayById(CUP_SELECT, 0.0f);
            this->prevPageId = lastPage;
        }
        else
        {
            this->LoadPrevPageWithDelayById(CUP_SELECT, 0.0f);
        }
    }
}