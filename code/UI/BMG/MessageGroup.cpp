#include <kamek.hpp>
#include <game/UI/Text.hpp>
#include <game/UI/Layout/Layout.hpp>
#include <core/rvl/os/OS.hpp>
#include <Settings/UserData.hpp>
#include <core/System/SystemManager.hpp>

static BMGHolder * AdditionalHolder = new(BMGHolder);

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

void InjectAdditionalHolder(BMGHolder * baseHolder, char * filename)
{
    baseHolder->Load(filename);

    using namespace DXData;

    char baseName[] = "DXExtra";
    char finalName[0x10];
    char testName[0x30];
    char suffix[0x6];

    char * localization = ArchiveRoot::sInstance->archivesHolders[ARCHIVE_HOLDER_UI]->archiveSuffixes[0x1];

    memset(suffix, 0, 0x6);
    strncpy(suffix, localization, strlen(localization)-4);
    snprintf(finalName, 0x10, "%s%s", baseName, suffix);
    snprintf(testName, 0x30, "message/%s%s", finalName, ".bmg");

    void * file = ArchiveRoot::sInstance->GetFile(ARCHIVE_HOLDER_UI, testName, 0x0);
    if(file != nullptr)
        AdditionalHolder->Load(finalName);
    else
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
