#ifndef _OS_MODULE_
#define _OS_MODULE_

#include <kamek.hpp>

class OSModuleInfo {
public:
    u32 id;
    void* linkPrev;
    void* linkNext;
    u32 sectionCount;
    u32 sectionInfoOffset;
    u32 nameOffset;
    u32 nameSize;
    u32 version;
};

class OSModuleHeader {
public:
    OSModuleInfo info;
    u32 bssSize;
    u32 relOffset;
    u32 impOffset;
    u32 impSize;
    s8 prologSection;
    s8 epilogSection;
    s8 unresolvedSection;
    s8 bssSection;
    u32 prolog;
    u32 epilog;
    u32 unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
};

static_assert(sizeof(OSModuleHeader) == 0x4C, "OSModuleHeader");

#endif