/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2025 Gabriela Orzechowska
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
 * 
 * This class is based on MKW-SP implementation of LZMA compression
 * https://github.com/mkw-sp/mkw-sp/blob/main/stub/LZMA.hh
 * 
 * The original license text goes as follows:
 * 
 * Copyright 2021-2023 Pablo Stebler
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
