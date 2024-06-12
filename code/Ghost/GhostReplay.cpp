#include <game/System/identifiers.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <kamek.hpp>
#include <game/Race/RaceData.hpp>
#include <game/UI/Page/RaceHUD/TTInterface.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>

int MakePlayerLocal(RacedataPlayer& player, int id){
    PlayerType type = RaceData::GetStaticInstance()->racesScenario.players[id].playerType;
    if(id == 0 && type == PLAYER_GHOST) return PLAYER_REAL_LOCAL;
    return type;
}

kmCall(0x80594444, MakePlayerLocal);
kmWrite32(0x80594434, 0x889F0010);
kmWrite32(0x80594448, 0x2c030000);
kmWrite32(0x80594450, 0x2c030001);
kmWrite32(0x80594458, 0x2c030003);

kmWrite32(0x80630450, 0x3880000d);
kmWrite32(0x80630474, 0x3880000d);
kmWrite32(0x80630498, 0x3880000d);

PageId GetProperTTPausePage(Pages::RaceHUD& hud) {
    MenuId id = MenuData::GetStaticInstance()->GetCurrentScene()->menuId;
    if(id >= WATCH_GHOST_FROM_CHANNEL && id <= WATCH_GHOST_FROM_MENU) return GHOST_REPLAY_PAUSE_MENU;
    return hud.GetPausePageId();
}
kmCall(0x808569e0, GetProperTTPausePage);

