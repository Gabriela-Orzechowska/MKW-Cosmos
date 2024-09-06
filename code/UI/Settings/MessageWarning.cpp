#include <UI/Settings/MessageWarning.hpp>
#include <UI/BMG/BMG.hpp>
#include <main.hpp>

namespace CosmosUI
{
    void MessagePageWindow::OnInit()
    {
        Approve::OnInit();
        this->SetMessageWindowMessage(0x0, 0x0);
        this->SetApproveButtonMessage(0x17e9, nullptr);
        this->onBackPressHandler.ptmf = static_cast<void (Page::*)(u32)> (&MessagePageWindow::OnBackPress);
        this->onClickHandler.ptmf = static_cast<void (Page::*)(PushButton*,u32)> (&MessagePageWindow::OnClick);
        this->messageCount = 0;
    }

    void MessagePageWindow::OnBackPress(u32 hudSlotId)
    {
        if(type[0] == RESTART_REQUIRED){
            this->nextPageId = (PageId) Cosmos::SETTINGS_MAIN;
        }
        else if(type[0] == SHUTDOWN){
            Cosmos::System::Shutdown();
        }
        
        this->NextMessage();
        //this->EndStateAnimate(0.0f, 1);
    }

    void MessagePageWindow::OnClick(PushButton *button, u32 hudSlotId)
    {
        if(type[0] == RESTART_REQUIRED){
            Cosmos::System::Restart();
        }
        else if(type[0] == SHUTDOWN){
            Cosmos::System::Shutdown(true);
        }
        this->NextMessage();
        //this->EndStateAnimate(0.0f,0);
    }

    void MessagePageWindow::NextMessage(){
        for(int i = 0; i < messageCount; i++){
            this->type[i] = this->type[i+1];
            this->messageBmg[i] = this->messageBmg[i+1];
        }

        this->messageCount--;
        switch(this->messageCount){
            case 0:
                this->EndStateAnimate(0.0f,0); break;
            case 1:
                this->SetApproveButtonMessage(0x17e9, nullptr);
                this->SetMessageWindowMessage(this->messageBmg[0],0x0); break;
            default:
                this->SetApproveButtonMessage(0x1b5b, nullptr);
                this->SetMessageWindowMessage(this->messageBmg[0],0x0); break;
        }

    };

    void MessagePageWindow::AddMessage(MESSAGE_TYPE type, u32 bmgid) {
        this->type[this->messageCount] = type;
        this->messageBmg[this->messageCount++] = bmgid;
        this->SetMessageWindowMessage(this->messageBmg[0],0x0);
        if(this->messageCount > 1) this->SetApproveButtonMessage(0x1b5b, nullptr);
    };

    void AwaitPageWithBackground::OnInit(){
        this->nextPageId = (PageId) -1;
        this->InitControlGroup(4);
        this->SetManipulatorManager(&this->manipulatorManager);
        this->AddControl(3, &this->blackBack, false);
        this->AddControl(1, &this->busySymbol, false);
        this->AddControl(2, &this->messageWindowScale, false);
        this->AddControl(0, &this->titleText, false);

        this->titleText.Load(0);
        this->busySymbol.Load("message_window", "BusySymbol", "BusySymbol");
        this->messageWindowScale.Load("message_window", "MessageWindow", "MessageWindowPopup");
        this->blackBack.Load("message_window", "BlackBack", "BlackBack");
        this->blackBack.zIndex = -1.0f;


    }
}
