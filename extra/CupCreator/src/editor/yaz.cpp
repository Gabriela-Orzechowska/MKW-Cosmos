#include "yaz.h"

template<typename T>
inline T Swap(T val){
    union {
        T val;
        unsigned  char u8[sizeof(T)];
    } source, dest;

    source.val = val;
    for(size_t i = 0; i < sizeof(T); i++){
        dest.u8[i] = source.u8[sizeof(T) - i - 1];
    }
    return dest.val;
}

std::vector<uint8_t> Decompress(std::vector<uint8_t>& compressed_data){
    if(compressed_data[0] != 'Y' || compressed_data[1] != 'a' || compressed_data[2] != 'z' || compressed_data[3] != '0'){
        return compressed_data;
    }
    uint32_t decompressed_size = (compressed_data[4] << 24) | (compressed_data[5] << 16) | (compressed_data[6] << 8) | (compressed_data[7] << 0);

    int inpos = 16;
    std::vector<uint8_t> output = {};

    while(output.size() < decompressed_size){
        uint8_t mask = compressed_data[inpos++];
        for(int i = 0; i < 8; i++){
            if((mask & 0x80) == 0){
                if(output.size() >= decompressed_size) break;
                uint8_t val = compressed_data[inpos++];
                uint32_t offset = (((val & 0x0F) << 8) | compressed_data[inpos++]) + 1;
                uint32_t length = (val & 0xF0) == 0 ? compressed_data[inpos++] + 0x12 : ((val & 0xF0) >> 4) + 2;

                for(int j = 0; j < length; j++) {
                    output.push_back(output[output.size() - offset]);
                }
            }
            else {
                output.push_back(compressed_data[inpos++]);
            }
            mask = mask << 1;
        }
    }
    return output;
}