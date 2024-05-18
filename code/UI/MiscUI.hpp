#ifndef _MISCUI_
#define _MISCUI_
#include <kamek.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>
#include <game/Sound/RSARSounds.hpp>


static bool loadedAsKorean = false;

namespace CosmosUI
{
    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const char * tpl);
    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const void * tplPointer);
}

static bool isOnlinePaused = false;

#endif