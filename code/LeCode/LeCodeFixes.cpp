#include <kamek.hpp>
#include <game/System/Archive.hpp>

// Dont preload tracks
kmWrite32(0x80541B58, 0x4E800020);

//Disable cup selection
void DisableCupSelection()
{
    extern u32 p_lecodeCup1; //0x80627a3c
    extern u32 p_lecodeCup2; //0x807e5c54
    extern u32 p_lecodeCup3; //0x80841614
    extern u32 p_lecodeCup4; //0x807e4740
    extern u32 p_lecodeCupCount; //0x807e5c1c

    p_lecodeCup1 = 0x4E800020; //blr
    p_lecodeCup2 = 0x4E800020; //blr
    p_lecodeCup3 = 0x9421ff20; //stwu sp, -0xE0 (sp)
    p_lecodeCup4 = 0x4E800020; //blr
    p_lecodeCupCount = 0x2c1c0008; //cmpwi r28, 0x8
    return;
}

static LeCodeLoadHook lclhDisableCupSelection(DisableCupSelection);


//kmWrite32(0x806394f0, 0x60000000);
