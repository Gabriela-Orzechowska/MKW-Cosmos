#include <UI/BMG/MessageGroup.hpp>

static BMGHolder * AdditionalHolder = nullptr;

static char * suffixes[13] = {
    "",
    "",
    "",
    "_G",
    "_F",
    "_Q",
    "_S",
    "_M",
    "_I",
    "_H",
    "_PL",
    "_J",
    "_K",
};

void CreateAdditionalHolder(){
    AdditionalHolder = new (BMGHolder);
}
static BootHook bhCreateAdditionalHolder(CreateAdditionalHolder, LOW);

void InjectAdditionalHolder(BMGHolder& baseHolder, char * filename)
{
    baseHolder.Load(filename);

    using namespace Cosmos::Data;

    char baseName[] = "CosmosExtra";
    char finalName[0x10];
    char testName[0x30];
    char suffix[0x6];

    char * localization = ArchiveRoot::GetStaticInstance()->GetHolder(ARCHIVE_HOLDER_UI)->archiveSuffixes[0x1];

    memset(suffix, 0, 0x6);
    strncpy(suffix, localization, strlen(localization)-4);
    snprintf(finalName, 0x10, "%s%s", baseName, suffix);
    snprintf(testName, 0x30, "message/%s%s", finalName, ".bmg");

    void * file = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, testName, 0x0);
    if(file != nullptr)
        AdditionalHolder->Load(finalName);
    else
        AdditionalHolder->Load("CosmosExtra");
    return;
}

kmCall(0x805c34a4, InjectAdditionalHolder);

void TextBox_setMessage_Patch(nw4r::lyt::TextBox * pane, BMGHolder * contextHolder, BMGHolder * commonHolder, u32 id, u32 * messInfo)
{
    if(AdditionalHolder->bmgFile != nullptr)
    {
        if(AdditionalHolder->GetMsgId(id) >= 0) 
        {
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
            PaneManager::SetTextBoxMessage(pane, AdditionalHolder, AdditionalHolder, id);
            return;
        }
            
    }
    PaneManager::SetTextBoxMessage(pane, contextHolder, commonHolder, id);
    return;
}


kmCall(0x8063decc, setTextBoxMessage_Patch);
kmCall(0x8063dd90, setTextBoxMessage_Patch);

int deleteElements(wchar_t * msg, int index, int size, int len)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = index; j < len+1; j++)
        {
            msg[j] = msg[j+1];
        }
    }
    return len-size;
}

void RemoveEscapeSequences(wchar_t * msg, int len)
{
    u8 index = 0;
    for(wchar_t * i = msg; *i; i++)
    {
        if(*i == L'\x1a')
        {
            u16 val = i[0x1];
            u8 size = val >> 9;
            deleteElements(msg, index, size, len);
        }
        index++;
    }
}

void GetTextFromMessage(char * out, u32 messageId)
{
    s32 slot = AdditionalHolder->GetMsgId(messageId);
    if(slot < 0)
        return;
    wchar_t * msg = AdditionalHolder->GetMsgByMsgId(slot);

    for(wchar_t * i = (wchar_t *) msg; *i != 0x0; i++)
    {
        if(*i == L'\x1a')
        {
            u16 val = i[0x1];
            u8 size = (val >> 9) - 2;
            for(int j = 0; j < size; j++)
                i[0x2+j] = (wchar_t) 0xFFFF;
        }
    }

    u32 mLen = wcslen(msg);
    RemoveEscapeSequences(msg,mLen);

    memset(out, 0x0, 128);
    wcstombs(out, msg, mLen+1);

    return;
}