//
// Created by Gabi on 3/24/2024.
//

#ifndef UIEDITOR_LYT_TYPES_H
#define UIEDITOR_LYT_TYPES_H

#include <cstdint>
#include "lmath.h"

namespace gablibela {
    namespace low {
        namespace lyt {
            const   int     ResourceNameStrMax  = 16;
            const   int     MaterialNameStrMax  = ResourceNameStrMax + 4;
            const   int     UserDataStrMax      =  8;
            const   int     TexMtxMax           = 10;
            const   int     TevColorMax         =  3;
            const   int     TevKColorMax        =  4;
            const   int     IndTexMtxMax        =  3;

            struct Size {
                float width;
                float height;
            };

            inline bool operator== (const Size& a, const Size& b){
                return a.width == b.width && a.height == b.height;
            }

            struct ChanCtrl {
                uint8_t    matSrcCol;
                uint8_t    matSrcAlp;
                uint8_t    reserve1;
                uint8_t    reserve2;
            };

            struct TexSRT {
                math::VEC2 translate;
                float      rotate;
                math::VEC2 scale;
            };

            struct TexCoordGen {
                uint8_t   texGenType;
                uint8_t   texGenSource;
                uint8_t   texMtx;
                uint8_t   reserved;
            };

            struct IndirectStage {
                uint8_t  texCoordGen;
                uint8_t  texMap;
                uint8_t  scaleS;
                uint8_t  scaleT;
            };

            struct TevStageInOp {
                uint8_t ab;
                uint8_t cd;
                uint8_t op;
                uint8_t cl;
            };

            struct TevStage {
                uint8_t       texCoordGen;
                uint8_t       colChan;
                uint8_t       texMap;
                uint8_t       swapSel;
                TevStageInOp  colIn;
                TevStageInOp  alpIn;
                uint8_t       indStage;
                uint8_t       indBiMt;
                uint8_t       indWrap;
                uint8_t       indfoAdUtAl;
            };

            struct TevSwapMode {
                uint8_t swap;
            };

            struct AlphaCompare {
                uint8_t comp;
                uint8_t op;
                uint8_t ref0;
                uint8_t ref1;
            };

            struct BlendMode {
                uint8_t type;
                uint8_t srcFactor;
                uint8_t dstFactor;
                uint8_t op;
            };

            struct InflationLRTB{
                float l;
                float r;
                float t;
                float b;
            };

            struct WindowFrameSize{
                float l;
                float r;
                float t;
                float b;
            };

            enum
            {
                PANEFLAG_VISIBLE,
                PANEFLAG_INFLUENCEDALPHA,
                PANEFLAG_LOCATIONADJUST,

                PANEFLAG_MAX
            };



        }
    }
}

#endif //UIEDITOR_LYT_TYPES_H
