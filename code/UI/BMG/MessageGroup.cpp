#include <kamek.hpp>
#include <game/UI/Text.hpp>
#include <game/UI/Layout/Layout.hpp>
#include <core/rvl/os/OS.hpp>

static BMGHolder * AdditionalHolder = new(BMGHolder);

void InjectAdditionalHolder(BMGHolder * baseHolder, char * filename)
{
    baseHolder->Load(filename);
    AdditionalHolder->Load("DXExtra");
    return;
}

kmCall(0x805c34a4, InjectAdditionalHolder);

void TextBox_setMessage_Patch(nw4r::lyt::TextBox * pane, BMGHolder * contextHolder, BMGHolder * commonHolder, u32 id, u32 * messInfo)
{
    if(AdditionalHolder->bmgFile != nullptr)
    {
        if(AdditionalHolder->GetMsgId(id) >= 0) 
        {
            OSReport("[DX] Patching MessageGroup ID: %d\n", id);
            PaneManager::TextBox_setMessage(pane, AdditionalHolder, AdditionalHolder, id, messInfo);
            return;
        }        
    }
    PaneManager::TextBox_setMessage(pane, contextHolder, commonHolder, id, messInfo);
    return;
}

kmCall(0x8063deb0, TextBox_setMessage_Patch);
kmCall(0x8063dd08, TextBox_setMessage_Patch);

void setTextBoxMessage_Patch(nw4r::lyt::TextBox * pane, BMGHolder * contextHolder, BMGHolder * commonHolder, u32 id)
{
    if(AdditionalHolder->bmgFile != nullptr)
    {
        if(AdditionalHolder->GetMsgId(id) >= 0) 
        {
            OSReport("[DX] Patching MessageGroup ID: %d\n", id);
            PaneManager::SetTextBoxMessage(pane, AdditionalHolder, AdditionalHolder, id);
            return;
        }
            
    }
    PaneManager::SetTextBoxMessage(pane, contextHolder, commonHolder, id);
    return;
}


kmCall(0x8063decc, setTextBoxMessage_Patch);
kmCall(0x8063dd90, setTextBoxMessage_Patch);
