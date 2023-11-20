#include <kamek.hpp>

typedef struct {
    u8 unknown[0x60];
} NETSHA1Context;

extern void NETSHA1Init(NETSHA1Context *context);
extern void NETSHA1Update(NETSHA1Context *context, const void *input, u32 length);
extern void NETSHA1GetDigest(NETSHA1Context *context, void *digest);

namespace SHA1
{
    char * GetFileSha1(void * buffer, u32 size);
}

