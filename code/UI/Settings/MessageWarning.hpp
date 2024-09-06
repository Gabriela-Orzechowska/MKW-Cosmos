#include "UI/Ctrl/MessageWindowControl.hpp"
#include <kamek.hpp>
#include <game/UI/Page/Other/Approve.hpp>
#include <game/UI/Page/Page.hpp>

namespace CosmosUI
{
    #define MAX_MESSAGE_COUNT 8

    enum MESSAGE_TYPE {
        RESTART_REQUIRED,
        INFO,
        SHUTDOWN,
    };

    class MessagePageWindow : public Pages::Approve
    {
    public:
        void OnInit() override;
        void OnBackPress(u32 hudSlotId);
        void OnClick(PushButton *button, u32 hudSlotId);
        void AddMessage(MESSAGE_TYPE type, u32 bmgid);

        
    private:
        void NextMessage();

        MESSAGE_TYPE type[MAX_MESSAGE_COUNT];
        u32 messageBmg[MAX_MESSAGE_COUNT];
        u32 messageCount;
    };

    class AwaitPageWithBackground : public Pages::AutoEnding {
    public:
        void OnInit() override;
        void SetMessageWindowText(u32 bmgId, const TextInfo *text) override {
            this->messageWindowScale.SetMsgId(bmgId, text);
        };

    protected:
        MessageWindowControlScaleFade messageWindowScale; 
        BlackBackControlForMessage blackBack;
    };
}
