/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <UI/BMG/MessageGroup.hpp>
#include <UI/Language/LanguageManager.hpp>

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

    char* suffix = suffixes[Cosmos::LanguageManager::GetStaticInstance()->GetActualLanguage()];

    snprintf(finalName, 0x10, "%s%s", baseName, suffix);
    snprintf(testName, 0x30, "message/%s%s", finalName, ".bmg");

    void * file = ArchiveRoot::GetStaticInstance()->GetFile(ARCHIVE_HOLDER_UI, testName, 0x0);
    if(file != nullptr)
        AdditionalHolder->Load(finalName);
    else
        AdditionalHolder->Load(baseName);
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

// DO NOT FUCKING ASK WHAT I DID HERE
u64 GetSlot_Patch(BMGHolder& holder, u32 bmgId){
    s32 ret = AdditionalHolder->GetMsgId(bmgId);
    BMGHolder* pointerToHolder = AdditionalHolder;
    if( ret == -1 ) {
        ret = holder.GetMsgId(bmgId);
        pointerToHolder = &holder;
    }

    return ((u64)ret) << 32 | (u32)pointerToHolder;
}
kmCall(0x805ce190, GetSlot_Patch);

asm int GetSlot_PatchCopy(){
    ASM(
        nofralloc;
        mr r20, r4;
        mr r21, r3;
        blr;
    )
}
kmCall(0x805ce194, GetSlot_PatchCopy);



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

wchar_t* GetMessageFromAdditionalHolder(u32 bmg){
    s32 slot = AdditionalHolder->GetMsgId(bmg);
    if(slot < 0)
        return nullptr;
    return AdditionalHolder->GetMsgByMsgId(slot);
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
