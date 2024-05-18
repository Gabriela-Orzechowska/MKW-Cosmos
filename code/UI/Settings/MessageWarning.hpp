#include <kamek.hpp>
#include <game/UI/Page/Other/Approve.hpp>
#include <game/UI/Page/Page.hpp>

namespace CosmosUI
{
    enum MESSAGE_TYPE {
        RESTART_REQUIRED,
        INFO,
    };

    class MessagePageWindow : public Pages::Approve
    {
    public:
        void OnInit() override;
        void OnBackPress(u32 hudSlotId);
        void OnClick(PushButton *button, u32 hudSlotId);
        void Setup(MESSAGE_TYPE type, u32 bmgid);

        
    private:
        MESSAGE_TYPE type;
    };
}
