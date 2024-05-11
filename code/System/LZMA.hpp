#pragma once
#include <kamek.hpp>
#include <vendor/lzma/LzmaDec.h>
#include <core/egg/Heap.hpp>

namespace Cosmos
{
    namespace Compression
    {
        class LZMA {
            public:
            static void* Decompress(u8* buffer, u32 len, EGG::Heap* heap);

        
            static void* Alloc(ISzAllocPtr alloc, size_t len) { return currentHeap->alloc(len, 0x4); }
            static void Free(ISzAllocPtr alloc, void* addr) { return currentHeap->free(addr); }
            static u32 GetSize() { return returnSize; }

            static EGG::Heap* currentHeap;
            static u32 returnSize;
        };
    } // namespace Compression
    
} // namespace Cosmos
