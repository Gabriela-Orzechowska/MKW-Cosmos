#ifndef _BIT_OPS_H
#define _BIT_OPS_H

#include <kamek.hpp>

static inline u16 u8array_to_u16(const u8 * array, int offset)
{
    register u16 result;
    register u32 addr = (u32)array + offset;
    asm{
        ASM(
            lhbrx result, addr;
        )
    }
    return result;
}


#endif