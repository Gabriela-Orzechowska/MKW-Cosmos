#include <kamek.hpp>
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/rvl_sdk.hpp>

#define LECODE_FILE "/rel/lecode-%s.bin"

typedef struct lectHeader {
    u32 magic;
    u32 version;
    u32 buildNumber;
    void * baseAddress;
    void *  entryPoint;
    u32 totalSize;
    u32 offsetParam;
    char region;
    char flag;
    char lecodeVersion;
} lectHeader;