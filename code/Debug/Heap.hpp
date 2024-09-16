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
