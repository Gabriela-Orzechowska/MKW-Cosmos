#include <kamek.hpp>
#include <game/Race/Kart/KartStatus.hpp>

// No Drift Threshold
kmWrite32(0x808b5b1c, 0x00);

// Patch Offroad Glitch [vabold]
void EndJumpPad(KartStatus * status)
{
    status->bitfield0 &= ~BITFIELD0::JUMP_PAD;
    status->bitfield2 &= ~BITFIELD2::JUMP_PAD_FIXED_SPEED;
    return;
}

kmCall(0x8058267c, EndJumpPad);