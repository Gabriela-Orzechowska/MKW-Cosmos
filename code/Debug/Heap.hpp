#pragma once
#include <kamek.hpp>
#include <main.hpp>
#include <core/egg/Heap.hpp>

namespace Cosmos {
    namespace Heap {
        class SafeAllocator {
        public:
            enum ALLOC_RET {
                SUCCESS = 0x0,
                HEAP_LOCKED = -0x1,
                FAIL = -0x2,
            };

            static s32 Alloc(void** out, u32 size) { return Alloc(out, size, 0x4, nullptr); } 
            static s32 Alloc(void** out, u32 size, u32 alignment) { return Alloc(out, size, alignment, nullptr); }
            static s32 Alloc(void** out, u32 size, EGG::Heap* heap) { return Alloc(out, size, 0x4, heap); }
            static s32 Alloc(void** out, u32 size, u32 alignment, EGG::Heap* heap);
            static bool VerifyAlloc() { return throwAllocFail; }
        private:
            static bool throwAllocFail;
        };
    }
}