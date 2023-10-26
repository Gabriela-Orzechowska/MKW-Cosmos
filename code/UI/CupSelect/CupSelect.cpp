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

    void CupSelectPlus::OnActivate()
    {
        Pages::CupSelect::OnActivate();
        this->bottomText->SetMsgId(0x2810 + (u32)currentLayout);
    }

    void CupSelectPlus::OnSwitchPress(u32 slotId)
    {        
        currentLayout = static_cast<CUP_LAYOUT>(((u32)currentLayout) ^ 0x1);

        DVDFileInfo fileHandle;
        char * filepath = "/bin/cup2.bin";

        char buffer[0x20] __attribute__ ((aligned(0x20)));

        if(DVDOpen(filepath, &fileHandle))
        {
            u32 offset = ((u32)currentLayout) * CUPFILE_SORT_OFFSET;

            if(DVDReadPrio(&fileHandle, buffer, 0x10, offset, 0x2))
            {
                LeCode::cup2_header * header = (LeCode::cup2_header * ) &buffer;

                u32 num_bytes = 0x10 + 0x10 * (header->course_cups);
                char actualBuffer[0x1000] __attribute__ ((aligned(0x20)));

                char * address = (char *) LeCode::LeCodeManager::GetStaticInstance()->GetCupParamAddress();

                DVDReadPrio(&fileHandle, actualBuffer, num_bytes, offset, 0x2);
                DVDClose(&fileHandle);
                memcpy(address, actualBuffer, num_bytes);
            }
            else {
                return;
            }
        }
        else return;

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