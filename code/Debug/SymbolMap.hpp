#pragma once

#include <kamek.hpp>
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/os/OS.hpp>
#include <core/egg/Heap.hpp>

namespace CosmosDebug
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
 
    typedef struct KamekSymbolEntry
    {
        u32 offset;
        u32 nameOffset;
    } KamekSymbolEntry;

    class SymbolManager{
        public:
            static void CreateStaticInstance();
            static void DestroyStaticInstance();
            static char * GetSymbolName(u32 address);

            static SymbolManager * sInstance;
            u32 kamekBaseAddress;
            SymbolMapHeader header;
            SymbolMapHeader kamekHeader;
            SymbolEntry * symEntryArray; //Array
            KamekSymbolEntry * kamekSymEntryArray; //Array
            char * symName;
            char * symNameTable;
            char * kamekSymNameTable;
        private:
            
    };
}