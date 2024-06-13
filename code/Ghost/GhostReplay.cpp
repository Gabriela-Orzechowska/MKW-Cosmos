#include <kamek.hpp>
#include <game/System/identifiers.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <game/UI/Page/RaceHUD/TTInterface.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>
#include <game/Race/Kart/Kart.hpp>

int MakePlayerLocal(RacedataPlayer& player){
    register u32 id;
    asm { ASM(mr id, r0);}
    PlayerType type = RaceData::GetStaticInstance()->racesScenario.players[id].playerType;
    return id == 0 && type == PLAYER_GHOST ? PLAYER_REAL_LOCAL : type;
}
kmCall(0x80594444, MakePlayerLocal);

int FixTransparency(const Kart& kart){
    u32 playerIdx = kart.values->playerIdx;
    PlayerType type = RaceData::GetStaticInstance()->racesScenario.players[playerIdx].playerType;
    return playerIdx == 0 && type == PLAYER_GHOST ? PLAYER_REAL_LOCAL : type;
}
kmCall(0x8058e264, FixTransparency);

//Use r3 for comparing instead
kmWrite16(0x80594448, 0x2c03);
kmWrite16(0x80594450, 0x2c03);
kmWrite16(0x80594458, 0x2c03);
//Activate TT interface
kmWrite32(0x80630450, 0x3880000d);
kmWrite32(0x80630474, 0x3880000d);
kmWrite32(0x80630498, 0x3880000d);

kmWrite16(0x805e1e7A, 0x0000);

inline bool isWatchingGhost(){
    MenuId id = MenuData::GetStaticInstance()->GetCurrentScene()->menuId;
    return id >= WATCH_GHOST_FROM_CHANNEL && id <= WATCH_GHOST_FROM_MENU;
}

PageId GetProperTTPausePage(Pages::RaceHUD& hud) {
    return isWatchingGhost() ? GHOST_REPLAY_PAUSE_MENU : hud.GetPausePageId();
}
kmCall(0x808569e0, GetProperTTPausePage);



