/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
