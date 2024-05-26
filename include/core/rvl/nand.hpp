#ifndef _RVLNAND_
#define _RVLNAND_
#include "types.hpp"
#define NAND_MAX_PATH 64

struct NANDFileInfo{
    s32 fd;
    s32 origFd;
    char origPath[NAND_MAX_PATH];
    char tmpPath[NAND_MAX_PATH];
    u8 accType;
    u8 stage;
    u8 mark;
    u8 padding;
};

enum {
    NAND_RESULT_OK = 0,
    NAND_RESULT_ACCESS = -1,
    NAND_RESULT_ALLOC_FAILED = -2,
    NAND_RESULT_BUSY = -3,
    NAND_RESULT_NOEXISTS = -12,
    NAND_RESULT_FATAL_ERROR = -128,
};

enum {
    NAND_PERM_OTHER_READ = 0x01,
    NAND_PERM_OTHER_WRITE = 0x02,
    NAND_PERM_GROUP_READ = 0x04,
    NAND_PERM_GROUP_WRITE = 0x08,
    NAND_PERM_OWNER_READ = 0x10,
    NAND_PERM_OWNER_WRITE = 0x20,
    NAND_PERM_OTHER_MASK = NAND_PERM_OTHER_WRITE | NAND_PERM_OTHER_READ,
    NAND_PERM_GROUP_MASK = NAND_PERM_GROUP_WRITE | NAND_PERM_GROUP_READ,
    NAND_PERM_OWNER_MASK = NAND_PERM_OWNER_WRITE | NAND_PERM_OWNER_READ,
};

enum {
    NAND_ACCESS_READ = 0x1,
    NAND_ACCESS_WRITE = 0x2,
};

enum {
    NAND_SEEK_SET = 0,
};

static_assert(sizeof(NANDFileInfo) == 0x8c, "NANDFileInfo");
#endif