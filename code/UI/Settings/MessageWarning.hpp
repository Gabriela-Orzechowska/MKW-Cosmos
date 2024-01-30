#include <kamek.hpp>
#include <game/UI/Page/Other/Approve.hpp>
#include <game/UI/Page/Page.hpp>

namespace CosmosUI
{
    class MessagePageWindow : public Pages::Approve
    {
        public:
        void OnInit() override;
        void OnBackPress(u32 hudSlotId);
        void OnClick(PushButton *button, u32 hudSlotId);
        void SetMessage(u32 id);
    };
}
