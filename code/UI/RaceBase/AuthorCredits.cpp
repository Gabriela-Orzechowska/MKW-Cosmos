#include <UI/RaceBase/AuthorCredits.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <core/rvl/os/OS.hpp>

namespace CosmosUI{

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
        loader.Load("game_image", "CosmosCredits", "info_1", NULL);
        this->textBox_00 = this->layout.GetPaneByName("TextBox_00");
        u32 trackId = Cosmos::CupManager::GetStaticInstance()->GetTrackID();
        this->SetMsgId(BMGOFFSET + trackId, 0);
    }

}
