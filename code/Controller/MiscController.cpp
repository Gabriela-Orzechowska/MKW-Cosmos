#include <kamek.hpp>


// Y to Wheelie
kmBranchDefAsm(0x801afbf8, 0x801afbfc)
{
    nofralloc

    li r14, 0x880
    and r15, r14, r0
    cmpw r15, r14
    bne+ end
    sub r0, r0, r15
    li r15, 0x88
    add r0, r0, r15

    end:
    sth r0, 0(r4)

    blr
}
