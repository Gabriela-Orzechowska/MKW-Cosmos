#include <kamek.hpp>
#include <game/Race/Kart/KartStatus.hpp>

// No Drift Threshold
kmWrite32(0x808b5b1c, 0x00);

// Patch Offroad Glitch [vabold]
void EndJumpPad(KartStatus * status)
{
    status->bitfield0 &= ~0x40000000;
    status->bitfield2 &= ~0x100000;
    return;
}

kmCall(0x8058267c, EndJumpPad);