#ifndef UIEDITOR_BRLYT_H
#define UIEDITOR_BRLYT_H

#include <cstdint>
#include "lyt_types.h"

namespace gablibela {
    namespace low {
        namespace brlyt {
            using namespace lyt;



            struct header {
                uint32_t magic;
                uint16_t bom;
                uint16_t version;
                uint32_t size;
                uint16_t headerSize;
                uint16_t sectionCount;
            };

            struct section_header {
                uint32_t magic;
                uint32_t size;
            };

            struct lyt1 {
                section_header header;
                uint8_t isCentered;
                uint8_t padding[3];
                float width;
                float height;
            };

            struct usd1 {
                section_header header;
                uint16_t count;
                uint16_t padding;
            };


            struct usd1_entry {
                uint32_t name_offset;
                uint32_t data_offset;
                uint16_t length;
                uint8_t type;
                uint8_t padding;
            };

            struct txl1 {
                section_header header;
                uint16_t count;
                uint16_t padding;
            };

            struct txl1_entry {
                uint32_t name_offset;
                uint32_t padding;
            };

            struct mat1 {
                section_header header;
                uint16_t count;
                uint16_t padding;
                uint32_t offsets;
            };

            struct mat1_entry {
                char name[0x14];
                int16_t col1[4];
                int16_t col2[4];
                int16_t col3[4];
                uint8_t tCol1[4];
                uint8_t tCol2[4];
                uint8_t tCol3[4];
                uint8_t tCol4[4];
                uint32_t bitfield;
            };

            struct pan1 {
                section_header header;
                uint8_t flag;
                uint8_t basePos;
                uint8_t alpha;
                uint8_t padding;

                char name[ResourceNameStrMax];
                char userData[UserDataStrMax];
                math::VEC3 translate;
                math::VEC3 rotate;
                math::VEC2 scale;
                Size size;
            };

            struct pic1 : pan1 {
                uint32_t vtxCol[4];
                uint16_t matIdx;
                uint8_t  texCoordNum;
                uint8_t  padding;
            };

            struct txt1 : pan1 {
                uint16_t textBufBytes;
                uint16_t textStrBytes;

                uint16_t matIdx;
                uint16_t fontIdx;
                uint8_t  textPos;
                uint8_t  textAlign;
                uint16_t padding;

                uint32_t textStrOff;
                uint32_t textCol[2];
                Size fontSize;
                float charSpace;
                float lineSpace;
            };

            struct WindowFrame{
                uint16_t matIdx;
                uint8_t  textureFlip;
                uint8_t  padding;
            };

            struct WindowContent {
                uint32_t  vtxCols[4];
                uint16_t  matIdx;
                uint8_t  texCoordNum;
                uint8_t  padding;
            };

            struct wnd1 : pan1 {
                InflationLRTB inflation;
                uint8_t       frameNum;
                uint8_t       padding[3];

                uint32_t      contentOffset;
                uint32_t      frameOffsetTableOffset;
            };
        }
    }
}

#endif //UIEDITOR_BRLYT_H
