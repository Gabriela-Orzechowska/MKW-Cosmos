#include <UI/RaceBase/AuthorCredits.hpp>
#include <LeCode/LeCodeManager.hpp>
#include <core/rvl/os/OS.hpp>

namespace DXUI{

    u32 ControlAuthorCredits::Count()
    {
        return 1;
    }

    void ControlAuthorCredits::Create(Page * page, u32 index)
    {
        ControlAuthorCredits * credits = new(ControlAuthorCredits);
        page->AddControl(index, credits, 0);
        credits->Load();
    }

    static CustomControlBuilder AuthorPanel(ControlAuthorCredits::Count, ControlAuthorCredits::Create);

    void ControlAuthorCredits::Load()
    {
        this->hudSlotId = 0;
        ControlLoader loader(this);
        loader.Load("game_image", "credits", "info_1", NULL);
        this->textBox_00 = this->layout.GetPaneByName("TextBox_00");
        u32 trackId = LeCode::LeCodeManager::GetStaticInstance()->GetTrackID();
        // DXLog("Trying to display credits. Message ID: %d\n", trackId + BMGOFFSET);
        this->SetMsgId(BMGOFFSET + trackId, 0);
    }

}