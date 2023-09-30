#include <main.hpp>

namespace DX{

    //Wiimmfi Stuff
    using IOS::Open2ndInst;
    asm s32 Open(char *path, IOS::Mode mode){ 
        ASM(
            nofralloc;
            stwu sp, -0x0020 (sp);
            b Open2ndInst;
        )
    }


    //Unlock Everything Without Save [_tZ]
    kmWrite32(0x80549974,0x38600001);

    //Fast Menus [east]
    kmWrite32(0x8038900C, 0x00000000);
    kmWrite32(0x80635670, 0x38000000);
    kmWrite32(0x80896830, 0x00000000);

}