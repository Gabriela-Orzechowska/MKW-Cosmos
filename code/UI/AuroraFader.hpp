#pragma once

#include "core/egg/Fader.hpp"
#include "core/rvl/gx/GXStruct.hpp"
#include "core/rvl/tpl.hpp"
#include <kamek.hpp>

namespace Aurora {
    namespace UI {
        class LMUFader: public EGG::ColorFader{
        public:
            enum FaderType {
                VANILLA,
                LMU,
                LMU_TRACK,
                SWOOP,
            };

            LMUFader(EGG::ColorFader* fader);
            ~LMUFader() override {};
            void draw() override;

            void SetType(FaderType _type) { 
                this->type = _type; 
                if(_type == LMU)
                    this->totalFadeDuration = 35;
                else if(_type == LMU_TRACK)
                    this->totalFadeDuration = 55;
                else this->totalFadeDuration = 20;
            }
            void DrawReset();
            void DrawRect(s16 x, s16 y, s16 width, s16 height, GXColor color);
            void DrawSwoop();
            void DrawLMU();
            void DrawLogoQuad();
            void DrawTrackImage();
            void LoadSampleTPL();
            static inline float EaseInOut( float t ) {
                return 0.5 * (1 + sin( 3.1415926 * (t - 0.5) ) );
            }

        private:
            FaderType type;

            float mtx[4][4];
            float posMtx[3][4];
            float logo_scale[2];
            float logo_offset[2];

            TPLPalettePtr tpl;
            GXTexObj obj;
        };
    }
}
