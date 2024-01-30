#include <UI/Settings/MessageWarning.hpp>
#include <UI/BMG/BMG.hpp>
#include <UI/Settings/SettingsBasePage.hpp>
#include <main.hpp>

namespace CosmosUI
{
    void MessagePageWindow::OnInit()
    {
        Approve::OnInit();
        this->SetMessageWindowMessage(0x2840, 0x0);
        this->onBackPressHandler.ptmf = static_cast<void (Page::*)(u32)> (&MessagePageWindow::OnBackPress);
        this->onClickHandler.ptmf = static_cast<void (Page::*)(PushButton*,u32)> (&MessagePageWindow::OnClick);
    }

    void MessagePageWindow::OnBackPress(u32 hudSlotId)
    {
        this->nextPageId = (PageId) Cosmos::SETTINGS_MAIN;
        this->EndStateAnimate(0.0f, 1);
    }

    void MessagePageWindow::OnClick(PushButton *button, u32 hudSlotId)
    {
        Cosmos::Restart();
    }
    void MessagePageWindow::SetMessage(u32 id)
    {
        this->SetMessageWindowMessage(id,0x0);
    }
}