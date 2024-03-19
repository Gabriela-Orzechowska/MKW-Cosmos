#include <kamek.hpp>
#include <game/Race/Kart/KartStatus.hpp>
#include <game/Race/Kart/KartCollision.hpp>
#include <game/Race/RaceInfo.hpp>
#include <game/KMP/Controller.hpp>
#include <game/Race/Kart/KartPointers.hpp>

// Patch Offroad Glitch [vabold]
void EndJumpPad(KartStatus * status)
{
    status->bitfield0 &= ~BITFIELD0::JUMP_PAD;
    status->bitfield2 &= ~BITFIELD2::JUMP_PAD_FIXED_SPEED;
    return;
}

kmCall(0x8058267c, EndJumpPad);

// Coob
kmWrite32(0x8057186c, 0x7fe6fb78); //mr r6, r31
s32 CalcCoob(KMP::Controller * controller, const Vec3 * position, u32 firstId, KartCollision * col){
    s32 ret = controller->GetAreaAtPosition(position, firstId, 0xA);
    if(ret < 0) return -1;

    AREA * curArea = controller->area->pointArray[ret]->rawData;
    u16 set1 = curArea->setting1;
    u16 set2 = curArea->setting2;

    RaceinfoPlayer * player = RaceInfo::GetStaticInstance()->players[col->base.pointers->values->playerIdx];   
    
    if(curArea->routeId == 1) {
        u8 kcp = player->currentKCP;
        if(!(set1 == 0 && set2 == kcp) && !(set1 == 1 && set2 != kcp)) return -1;
    }
    else if(curArea->routeId == 0xFF){
        if(!(set1 == 0 && set2 == 0)){
            u16 cp = player->checkpoint;
            if(set2 > set1 ? (set2 <= cp && cp < set1) : (set2 > cp && cp >= set1)) return -1;
        }
    }
    return ret;
}
kmCall(0x80571870, CalcCoob);