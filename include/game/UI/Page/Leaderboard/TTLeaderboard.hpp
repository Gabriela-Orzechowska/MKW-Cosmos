#ifndef _TTLEADERBOARD_
#define _TTLEADERBOARD_
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Page/Leaderboard/Leaderboard.hpp>
#include <game/UI/Ctrl//CtrlRace/CtrlRaceResult.hpp>

namespace Pages{
class TTLeaderboard : public Leaderboard { //0x2E
public:
    TTLeaderboard(); //80625134
    ~TTLeaderboard() override; //8085e438 vtable 808daabc
    PageId GetNextPage() const override; //0x10 8085dc44
    void OnInit() override; //0x28 8085d564
    int GetRuntimeTypeInfo() const override; //0x60 8085e4b8
    bool CanEnd() override; //0x64 8085c504
    void LoadRows() override; //0x68 8085d78c
    void func_0x6c() override; //0x6c 8085c5a0 does something with ghosthandler if menuid and other conditions are met
    int GetRowCount() const override; //0x70 8085d784 returns 6
    
    CtrlRaceResult **results; //0x1A0, size of the array = row count
    u8 unknown_0x1A4[0x1A8-0x1A4];
    Timer fastLapPB; //0x1A8 fetched at 8085d5c8
    u32 staffGhostState; //1 = easy; 2 = expert unlocked 0x1B4
    LayoutUIControl ghostMessage; //0x1B8 "You've unlocked an Expert Staff Ghost Data record!"
}; //total size 0x32C
static_assert(sizeof(TTLeaderboard) == 0x32c, "TTLeaderboard");
}//namespace Pages

//8085d8d0 updates TTLeaderboardPage
#endif