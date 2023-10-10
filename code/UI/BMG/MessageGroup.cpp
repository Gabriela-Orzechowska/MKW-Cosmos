#include <kamek.hpp>
#include <game/UI/Text.hpp>

static BMGHolder * AdditionalHolder = nullptr;

void LoadAdditionalHolder(BMGHolder * baseHolder, char * filename)
{
    baseHolder->Load(filename);
    if(AdditionalHolder == nullptr)
    {
        AdditionalHolder = new(BMGHolder);
        AdditionalHolder->Load("DXExtra");
    }
    if(AdditionalHolder->bmgFile == nullptr)
    {
        AdditionalHolder->Load("DXExtra");
    } 
    return;
}

kmCall(0x805c34a4, LoadAdditionalHolder);

void DestroyAdditionalHolder(BMGHolder * baseHolder, u32 param_2)
{
    baseHolder->Destroy(param_2);
    AdditionalHolder->Destroy(param_2);
    return;
}

kmCall(0x8063e7d8, DestroyAdditionalHolder);
kmCall(0x8063d8ec, DestroyAdditionalHolder);

static bool useAdditionalHolder = false;

int AdditionalHolderGetMessage(BMGHolder * baseHolder, u32 messageId)
{
    u32 result = -1;
    if(AdditionalHolder != nullptr && AdditionalHolder->bmgFile != nullptr)
        result = AdditionalHolder->GetMsgId(messageId);
    useAdditionalHolder = result != -1;
    if(result == -1)
        result = baseHolder->GetMsgId(messageId);
        
    return result;
}

kmCall(0x805ceb6c, AdditionalHolderGetMessage);
kmCall(0x805cef48, AdditionalHolderGetMessage);
kmCall(0x805e7848, AdditionalHolderGetMessage);
kmCall(0x805ce190, AdditionalHolderGetMessage);
kmCall(0x805cdd7c, AdditionalHolderGetMessage);

wchar_t * AdditionalHolderGetMessageString(BMGHolder * baseHolder, u32 slot)
{
    wchar_t * result;
    if(useAdditionalHolder)
        result = AdditionalHolder->GetMsgByMsgId(slot);
    else
        result = baseHolder->GetMsgByMsgId(slot);

    return result;
}

kmCall(0x80637aac, AdditionalHolderGetMessageString);
kmCall(0x805eeab4, AdditionalHolderGetMessageString);
kmCall(0x805e7884, AdditionalHolderGetMessageString);
kmCall(0x805e7854, AdditionalHolderGetMessageString);
kmCall(0x805cef90, AdditionalHolderGetMessageString);
kmCall(0x805cebb4, AdditionalHolderGetMessageString);
kmCall(0x805ce1d8, AdditionalHolderGetMessageString);
kmCall(0x805cddc4, AdditionalHolderGetMessageString);