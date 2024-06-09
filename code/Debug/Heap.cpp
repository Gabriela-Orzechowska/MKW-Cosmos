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
