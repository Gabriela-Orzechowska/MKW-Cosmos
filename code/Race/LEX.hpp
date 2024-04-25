#pragma once
#include <kamek.hpp>

namespace Cosmos {
    namespace LEX {
        class Section {
            u32 magic;
            u32 size;
        };

        class SET1 : public Section {
            Vec3 itemPosFactor;
            u8 startItem;
            u8 padding;
            u16 onlineTime; 

            void ApplySettings();
        };


        class CANN : public Section {
            struct CannonSettings {
                float speed;
                float height;
                float decelerationFactor;
                float endDeceleration;
            };

            u32 cannonTypes;
            CannonSettings settings[1];
        };

        class HIPT : public Section { 
            
        };

    }
}