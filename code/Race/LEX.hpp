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
            struct HideSettings {
                u8 condition;
                s8 lap;
                u8 from;
                u8 to;
                u8 show;
            };
            HideSettings settings[1];
        };
    }
}
