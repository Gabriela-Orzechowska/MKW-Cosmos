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
#include <main.hpp>
#include <Debug/Heap.hpp>

namespace Cosmos
{
    namespace Heap {

        static void* CheckHeapAlloc(void* buffer) {
            if(buffer == nullptr && SafeAllocator::VerifyAlloc())
                Cosmos::Panic(__FILE__, __LINE__, "Out of Memory!");
            return buffer;
        } 

        kmBranch(0x80229ad0, CheckHeapAlloc);
        kmBranch(0x80198e34, CheckHeapAlloc);
        kmBranch(0x80199600, CheckHeapAlloc);
        kmBranch(0x80199ac0, CheckHeapAlloc);

        bool SafeAllocator::throwAllocFail = true;

        s32 SafeAllocator::Alloc(void** out, u32 size, u32 alignment, EGG::Heap* heap) {
            if(heap == nullptr) heap = EGG::Heap::sCurrentHeap;
            if(heap->isLocked()) return HEAP_LOCKED;
             
            throwAllocFail = false;
            *out = EGG::Heap::alloc(size, alignment, heap);
            throwAllocFail = true;
            return (*out) == nullptr ? FAIL : SUCCESS;
        }
    }
} // namespace Cosmos
