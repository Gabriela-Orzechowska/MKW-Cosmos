#pragma once 

#include <kamek.hpp>

namespace EGG {
    // Bloom settings are definitely a separate struct
    struct BloomBokeh {
        u8 unknown_0x0[0x2c-0x00];
        float bloom1Strength;
        u8 unknown_0x30[0x50-0x30];
        float bloom2Strength;
        u8 unknown_0x54[0x6c-0x54];
    };

    //Add Size
    struct Blur {
        u8 unknown_0x0[0x3c-0x00];
        float strength;

    };


    // This all is wrong, I just need to code something
    class PosteffectMgr {
    public:
        ~PosteffectMgr(); //vtable 802a2c50

        u8 unknown_0x0[0x1a-0x00];
        u8 copyCount;
        u8 unknown_0x1b;
        void* texThreshhold; //0x1c
        Blur* blur[2]; //0x20
        void* dof;
        BloomBokeh* bloom;
        void* tint;
        u8 unknown_0x2c[0xCC-0x30];
    };
}
