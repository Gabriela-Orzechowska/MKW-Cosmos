#ifndef _MISCUI_
#define _MISCUI_
#include <kamek.hpp>


static bool loadedAsKorean = false;

namespace CosmosUI
{
    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const char * tpl);
    void ChangePaneImage(LayoutUIControl * ctrl, const char * pane, const void * tplPointer);
}
#endif