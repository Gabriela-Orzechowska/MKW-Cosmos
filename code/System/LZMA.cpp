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
 * https://github.com/mkw-sp/mkw-sp/blob/main/stub/LZMA.cc
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

#include <System/LZMA.hpp>

#define DICT_BUF_SIZE 0x30000


namespace Cosmos {
    namespace Compression
    {
        EGG::Heap* LZMA::currentHeap = nullptr;
        u32 LZMA::returnSize = 0;

        void* LZMA::Decompress(u8* buffer, u32 len, EGG::Heap* heap) {
            if(len < (13)) return nullptr;
            currentHeap = heap;

            bool knownSize = true;
            bool ret = true;
            void* retBuffer = nullptr;

            CLzmaDec dec;
            LzmaDec_Construct(&dec);
            if(buffer[5] == 0xFF) knownSize = false;

            ISzAlloc alloc;
            alloc.Alloc = LZMA::Alloc;
            alloc.Free = LZMA::Free;

            ret = LzmaDec_AllocateProbs(&dec, buffer, LZMA_PROPS_SIZE, &alloc) == SZ_OK;
            if(!ret) {
                CosmosLog("Alloc failed\n");
                return nullptr;
            }
            LzmaDec_Init(&dec);
            dec.dicBufSize = DICT_BUF_SIZE;
            dec.dic = (Byte *) new (heap, 0x20) char[DICT_BUF_SIZE];

            buffer += 13;
            len -= 13;

            ELzmaStatus status;
            while(len != 0){
                if(dec.dicPos == dec.dicBufSize){
                    dec.dicBufSize += DICT_BUF_SIZE;
                    if(currentHeap->resizeForMBlock(dec.dic, dec.dicBufSize) != dec.dicBufSize){
                        u8* curDic = dec.dic;
                        dec.dic = new (currentHeap, 0x20) u8[dec.dicBufSize];
                        memcpy(dec.dic, curDic, dec.dicBufSize);
                        delete[] curDic;
                    }
                }
                size_t offset = len;
                if(LzmaDec_DecodeToDic(&dec, dec.dicBufSize, buffer, &offset, LZMA_FINISH_ANY, &status) != SZ_OK) {
                    goto cleanup;
                }
                buffer += offset;
                len -= offset;
                if(len != 0 && (status == LZMA_STATUS_FINISHED_WITH_MARK)){
                    goto cleanup;
                }
            }
            if(status == LZMA_STATUS_FINISHED_WITH_MARK) {
                if(currentHeap->resizeForMBlock(dec.dic, dec.dicPos) == dec.dicPos){
                    dec.dicBufSize = dec.dicPos;
                }
            }
            retBuffer = dec.dic;
            returnSize = dec.dicBufSize;
            cleanup:
            dec.dic = nullptr;
            dec.dicBufSize = 0;
            LzmaDec_FreeProbs(&dec, &alloc);
            return retBuffer;
        }
    } // namespace Compression
    
}
