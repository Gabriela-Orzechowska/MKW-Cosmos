#include <UI/Settings/MessageWarning.hpp>
#include <UI/BMG/BMG.hpp>
#include <main.hpp>

namespace CosmosUI
{
    void MessagePageWindow::OnInit()
    {
        Approve::OnInit();
        this->SetMessageWindowMessage(0x0, 0x0);
        this->onBackPressHandler.ptmf = static_cast<void (Page::*)(u32)> (&MessagePageWindow::OnBackPress);
        this->onClickHandler.ptmf = static_cast<void (Page::*)(PushButton*,u32)> (&MessagePageWindow::OnClick);
    }

    void MessagePageWindow::OnBackPress(u32 hudSlotId)
    {
        if(type == RESTART_REQUIRED){
            this->nextPageId = (PageId) Cosmos::SETTINGS_MAIN;
        }
        
        this->EndStateAnimate(0.0f, 1);
    }

    void MessagePageWindow::OnClick(PushButton *button, u32 hudSlotId)
    {
        if(type == RESTART_REQUIRED){
            Cosmos::System::Restart();
        }
        else {
            this->EndStateAnimate(0.0f,0);
        }
    }

    void MessagePageWindow::Setup(MESSAGE_TYPE type, u32 bmgid) {
        this->type = type;
        this->SetMessageWindowMessage(bmgid,0x0);
    };
}