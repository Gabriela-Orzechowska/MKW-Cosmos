#include <System/LZMA.hpp>

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
            dec.dicBufSize = 0x20000;
            dec.dic = (Byte *) new (heap, 0x20) char[0x20000];

            buffer += 13;
            len -= 13;

            ELzmaStatus status;
            while(len != 0){
                if(dec.dicPos == dec.dicBufSize){
                    dec.dicBufSize += 0x20000;
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