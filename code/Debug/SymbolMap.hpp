#pragma once

#include <kamek.hpp>
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/os/OS.hpp>
#include <core/egg/Heap.hpp>

namespace DXDebug
{
    typedef struct SymbolMapHeader
    {
        u32 magic;
        u32 filesize;
        u32 symbolCount;
        u32 nameTableOffset;
    } SymbolMapHeader;

    typedef struct SymbolEntry
    {
        u32 data1;
        u32 data2;
    } SymbolEntry;
 
    class SymbolManager{
        public:
            static void CreateStaticInstance();
            static void DestroyStaticInstance();
            static char * GetSymbolName(u32 address);

            static SymbolManager * sInstance;
            SymbolMapHeader header;
            SymbolEntry * symEntryArray; //Array
            char * symName;
            char * symNameTable;
        private:
            
    };
}