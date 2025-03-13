#include "SlotExpansion/CupManager.hpp"
#include "System/Identifiers.hpp"
#include "System/System.hpp"
#include "UI/MenuDataPlus.hpp"
#include "UI/MenuData/Scene.hpp"
#include "core/System/RKSystem.hpp"
#include "core/egg/Fader.hpp"
#include "core/nw4r/ut/Color.hpp"
#include "core/rvl/dvd/dvd.hpp"
#include "core/rvl/gx/GX.hpp"
#include "core/rvl/gx/GXStruct.hpp"
#include "core/rvl/gx/GXTexture.hpp"
#include "core/rvl/gx/GXEnum.hpp"
#include "core/rvl/mtx/mtx.hpp"
#include "core/rvl/tpl.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include "types.hpp"
#include <UI/AuroraFader.hpp>
#include <Aurora/UIAnimation.hpp>
#include <Debug/Heap.hpp>

namespace Aurora {
    namespace UI{
        static LMUFader* testFader = nullptr;

        LMUFader::LMUFader(ColorFader* fader): ColorFader(0.0f, 0.0f, 640.0f, 480.0f, nw4r::ut::Color(), EGG::Fader::CAN_FADE_IN)
        {
            this->status = fader->status;
            this->color = fader->color;

        }

        void LMUFader::DrawRect(s16 x, s16 y, s16 x2, s16 y2, GXColor color)
        {
            GXClearVtxDesc();
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);

            GXSetChanMatColor(GX_COLOR0A0, color);
            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            GX_Position2s16(x,y);
            GX_Position2s16(x2,y);
            GX_Position2s16(x2,y2);
            GX_Position2s16(x,y2);
            GXEnd();
        }

        void LMUFader::DrawReset(){
            C_MTXOrtho(mtx, 0.0f, 456.0f, 0.0f, 608.0f, 0.0f, -1.0f);
            PSMTXIdentity(posMtx);

            GXSetViewport(0.0f,0.0f,608.0f,456.0f,0.0f,1.0f);
            GXSetScissor(0,0, 608, 456);

            GXSetProjection(mtx, GX_ORTHOGRAPHIC);
            GXLoadPosMtxImm(posMtx,0);
            GXSetCurrentMtx(0);
        }


        const float logo_width = 640.0f;
        const float logo_height = 146.0f;

        static float coords_logo[] __attribute__((aligned(0x20))) = {
            //A
            0.0f, 127.8f, 45.44f, 120.9f, 39.72f, 146.0f, 0.0f, 146.0f,
            36.11f, 0.0f, 64.88f, 39.83f, 45.44f, 120.9f, 0.0f, 127.8f,
            36.11f, 0.0f, 98.45f, 0.0f, 68.85f, 39.83f, 64.88f, 39.83f,
            98.45f, 0.0f, 135.9f, 133.6f, 87.74f, 120.9f, 68.85f, 39.83f,
            87.84f, 120.9f, 135.9f, 133.6f, 135.9f, 146.0f, 93.76f, 146.0f,
            53.81f, 86.0f, 79.67f, 86.0f, 87.84f, 120.9f, 45.44f, 120.9f,
            //UR
            133.8f, 0.0f, 176.9f, 0.0f, 176.9f, 103.2f, 133.8f, 119.2f,
            133.8f, 119.2f, 176.9f, 103.2f, 182.9f, 109.1f, 160.6f, 146.0f,
            182.9f, 109.1f, 201.1f, 109.1f, 240.7f, 146.0f, 160.6f, 146.0f,
            201.1f, 109.1f, 207.0f, 103.4f, 240.7f, 129.1f, 240.7f, 146.0f,
            207.0f, 103.4f, 250.6f, 101.5f, 250.6f, 119.0f, 240.7f, 129.1f,
            207.0f, 67.66f, 240.7f, 67.66f, 250.6f, 101.5f, 207.0f, 103.4f,
            207.0f, 0.0f, 240.7f, 34.06f, 240.7f, 67.66f, 207.0f, 67.66f,
            207.0f, 0.0f, 291.9f, 0.0f, 267.5f, 34.06f, 240.7f, 34.06f,
            291.9f, 0.0f, 315.2f, 22.5f, 272.0f, 38.41f, 267.5f, 34.06f,
            272.0f, 38.41f, 315.2f, 22.5f, 315.5f, 76.7f, 271.8f, 63.57f,
            271.8f, 63.57f, 315.5f, 76.7f, 298.3f, 93.47f, 267.7f, 67.66f,
            240.7f, 67.66f, 267.7f, 67.66f, 256.1f, 101.5f, 250.6f, 101.6f,
            267.7f, 67.66f, 298.3f, 93.47f, 318.5f, 133.9f, 256.1f, 101.5f,
            256.1f, 101.5f, 318.5f, 133.9f, 318.5f, 146.0f, 276.7f, 146.0f,
            
            //OR
            317.3f, 26.03f, 343.6f, 0.0f, 367.0f, 35.01f, 360.9f, 41.1f,
            317.3f, 26.03f, 360.9f, 41.1f, 360.9f, 104.7f, 317.3f, 119.7f,
            317.3f, 119.7f, 360.9f, 104.7f, 367.1f, 111.0f, 343.2f, 146.0f,
            367.1f, 111.0f, 391.3f, 111.0f, 432.6f, 146.0f, 343.2f, 146.0f,
            397.3f, 105.1f, 432.6f, 127.9f, 432.6f, 146.0f, 391.3f, 111.0f,
            397.3f, 92.8f, 440.6f, 120.1f, 432.6f, 127.9f, 397.3f, 105.1f,
            397.3f, 81.0f, 440.6f, 101.6f, 440.6f, 120.1f, 397.3f, 92.8f,
            397.3f, 74.04f, 432.3f, 67.42f, 440.6f, 101.6f, 397.3f, 81.0f,
            397.3f, 40.89f, 432.3f, 34.27f, 432.3f, 67.42f, 397.3f, 74.04f,
            397.3f, 0.0f, 432.3f, 0.0f, 432.3f, 34.27f, 397.3f, 40.89f, 
            391.2f, 0.0f, 397.3f, 0.0f, 397.3f, 40.89f, 391.2f, 35.01f,
            432.3f, 0.0f, 483.7f, 0.0f, 459.5f, 34.27f, 432.3f, 34.27f,
            483.7f, 0.0f, 507.2f, 22.94f, 463.7f, 38.14f, 459.5f, 34.27f,
            463.7f, 38.14f, 507.2f, 22.94f, 507.2f, 77.01f, 463.7f, 63.39f,
            463.7f, 63.39f, 507.2f, 77.01f, 490.2f, 93.69f, 459.3f, 67.42f,
            432.3f, 67.42f, 459.3f, 67.42f, 447.9f, 101.6f, 440.6f, 101.6f,
            459.3f, 67.42f, 490.2f, 93.69f, 468.6f, 146.0f, 447.9f, 101.6f,
            490.2f, 93.69f, 504.1f, 121.4f, 504.1f, 146.0f, 468.6f, 146.0f,
            343.6f, 0.0f, 391.2f, 0.0f, 391.2f, 35.01f, 367.0f, 35.01f,

            //A2
            504.1f, 121.4f, 549.5f, 120.9f, 543.8f, 146.0f, 504.1f, 146.0f,
            540.2f, 0.0f, 569.0f, 39.83f, 549.5f, 120.9f, 504.1f, 121.4f,
            540.2f, 0.0f, 602.5f, 0.0f, 572.9f, 39.83f, 569.0f, 39.83f,
            602.5f, 0.0f, 640.0f, 133.6f, 591.9f, 120.9f, 572.9f, 39.83f,
            591.9f, 120.9f, 640.0f, 133.6f, 640.0f, 146.0f, 597.8f, 146.0f,
            557.7f, 86.0f, 583.9f, 86.0f, 591.9f, 120.9f, 549.5f, 120.9f,

        };

        void LMUFader::DrawLogoQuad(){
            GXColor white;
            GXColor dropShadow;
            white.rgba = 0xFFFFFFFF;
            dropShadow.rgba = 0;

            float prog = (this->color.a / 255.0f);

            int quad_count = 6 + 14 + 19 + 6;
            int vertex_count = quad_count * 4;

            prog = EaseInOut(prog);
            white.a = prog * 255.0f;
            dropShadow.a = prog * 128.0f;

            if(this->status == EGG::Fader::FADE_IN) {
                prog = 1.0f - prog;
                prog /= 0.7f;
                if(prog > 1.0f) prog = 1.0f;
                white.a = (1.0f-prog) * 255.0f;
                dropShadow.a = (1.0f-prog) * 128.0f;
            }

            this->logo_scale[0] = 0.5f;
            this->logo_scale[1] = 0.66f;
            if(this->status == EGG::Fader::FADE_OUT){
                this->logo_scale[0] *= 0.3f + (prog * 0.7f);
                this->logo_scale[1] *= 0.3f + (prog * 0.7f);
            }
            else if(this->status == EGG::Fader::FADE_IN){
                this->logo_scale[0] *= 1.0f + (prog * 0.8f);
                this->logo_scale[1] *= 1.0f + (prog * 0.8f);
            }

            this->logo_offset[0] = 304.0f - (0.5f * this->logo_scale[0] * 648.0f);
            this->logo_offset[1] = 228.0f - (0.5f * this->logo_scale[1] * 146.0f);

            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
            GXSetNumChans(1);
            GXSetNumIndStages(0);
            GXSetNumTexGens(0);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEX_DISABLE, GX_COLOR0A0);
            GXSetCullMode(GX_CULL_BACK);
            GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);

            GXClearVtxDesc();
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);

            GXSetChanMatColor(GX_COLOR0A0, dropShadow);

            GXBegin(GX_QUADS, GX_VTXFMT0, vertex_count);
            {
                for(int i = 0; i < 2 * vertex_count; i += 2)
                    GX_Position2f32(
                            (logo_scale[0] * coords_logo[i]) + logo_offset[0] + 3, 
                            (logo_scale[1] * coords_logo[i+1]) + logo_offset[1] + 3
                            );
            }   

            GXSetChanMatColor(GX_COLOR0A0, white);
            GXBegin(GX_QUADS, GX_VTXFMT0, vertex_count);
            {
                for(int i = 0; i < 2 * vertex_count; i += 2)
                    GX_Position2f32(
                            (logo_scale[0] * coords_logo[i]) + logo_offset[0], 
                            (logo_scale[1] * coords_logo[i+1]) + logo_offset[1]
                            );
            }   

            GXEnd();

        }
        static s16 coords[] __attribute__((aligned(0x20))) = {
            0,0,608,0,608,456,0,456,
        };
        static u8 tex_coords[] __attribute__((aligned(0x20))) = {
            0,0,1,0,1,1,0,1,
        };
        static u32 colors[] __attribute__((aligned(0x20))) = {
            0xFFFFFFFF,
        };

        void LMUFader::DrawTrackImage() {
            if(this->tpl == nullptr) return;
            this->DrawReset();
            TPLGetGXTexObjFromPalette(this->tpl, &this->obj, 0);
            GXLoadTexObj(&this->obj, GX_TEXMAP0);
            GXClearVtxDesc();
            GXSetNumChans(1);
            GXSetNumIndStages(0);
            GXSetNumTexGens(1);
            GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
            GXSetCullMode(GX_CULL_NONE);
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            // Add an indexed texture coordinate to the vertex description
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            GXSetArray(GX_VA_POS, coords, 2*sizeof(s16));
            GXSetArray(GX_VA_CLR0, colors, 1*sizeof(s32));
            GXSetArray(GX_VA_TEX0, tex_coords, 2*sizeof(u8));
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);


            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
            GXSetNumTevStages(1);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);

            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            {
                for(int i = 0; i < 4; i++){
                    GX_Position1x8(i);
                    GX_Color1x8(0);
                    GX_TexCoord1x8(i);
                }   
            }
            GXEnd();

        }

        void LMUFader::DrawLMU() {
            if(this->color.a == 0) return;
            float prog = (this->color.a / 255.0f);
            bool is_second = false;
            if(this->type == LMU_TRACK){
                prog *= 2.0f;
                if(prog > 1.0f) {
                    prog = 2.0f - prog;
                    is_second = true;
                }
            }

            prog = EaseInOut(prog);
            if(is_second) {
                prog = 0.3f + 0.7f * prog;
            this->DrawTrackImage();
                }
            this->DrawReset();
            //__GXInitGX();
            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
            GXSetNumChans(1);
            GXSetNumIndStages(0);
            GXSetNumTexGens(0);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEX_DISABLE, GX_COLOR0A0);
            GXSetCullMode(GX_CULL_NONE);
            GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
            GXSetNumTevStages(1);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);

            GXClearVtxDesc();
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
            GXColor test;
            test.rgba = -1U;
            if(Animator::GetStaticInstance())
                test.rgba = Animator::GetStaticInstance()->GetCurrentColor();
            GXSetChanMatColor(GX_COLOR0A0, test);

            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            {
                GX_Position2f32(-400.0f,0.0f);
                GX_Position2f32(408.0f*prog,0.0f);

                GX_Position2f32(408.0f*prog - 208.0f,456.0f);
                GX_Position2f32(-400.0f,456.0f);
            }   

            test.r /= 12;
            test.g /= 12;
            test.b /= 12;
            GXSetChanMatColor(GX_COLOR0A0, test);
            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            {
                GX_Position2f32(608.0f - ((408.0f * (prog)) - 208.0f),0.0f);
                GX_Position2f32(1000.0f,0.0f);
                GX_Position2f32(1000.f,456.0f);
                GX_Position2f32(608.0f - (408.0f * (prog)),456.0f);
            }   

            prog /= 8;
            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            {
                GX_Position2f32(-400.0f,0.0f);
                GX_Position2f32(408.0f*prog,0.0f);
                GX_Position2f32(408.0f*prog - 208.0f,456.0f);
                GX_Position2f32(-400.0f,456.0f);
            }   

            if(Animator::GetStaticInstance())
                test.rgba = Animator::GetStaticInstance()->GetCurrentColor();
            GXSetChanMatColor(GX_COLOR0A0, test);
            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            {
                GX_Position2f32(608.0f - ((408.0f * (prog)) - 208.0f),0.0f);
                GX_Position2f32(1000.0f,0.0f);
                GX_Position2f32(1000.f,456.0f);
                GX_Position2f32(608.0f - (408.0f * (prog)),456.0f);
            }   
            this->DrawLogoQuad();
        }

        void LMUFader::DrawSwoop(){
            this->DrawReset();
            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
            GXSetNumChans(1);
            GXSetNumIndStages(0);
            GXSetNumTexGens(0);
            GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEX_DISABLE, GX_COLOR0A0);
            GXSetCullMode(GX_CULL_BACK);
            GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
            GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVDSTALPHA, GX_LO_COPY);
            GXSetColorUpdate(GX_TRUE);
            GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            GXColor test;
            Animator* animator = Animator::GetStaticInstance(); 

            if(this->status == EGG::Fader::FADE_OUT) {
                float prog = (this->color.a / 255.0f);
                float prog_a = (prog / 0.6f);
                float prog_b = ((prog - 0.2f) / 0.6f);
                float prog_c = ((prog - 0.4f) / 0.6f);

                test.rgba = 0x78d6bfFF; 
                if(animator){
                    test.rgba = animator->GetCurrentColor();
                }
                this->DrawRect(0,0,(608 * prog_a), 456, test);

                test.rgba = 0x40767aFF; 
                if(animator){
                    test.rgba = animator->GetCurrentColor();
                    test.r /= 2;
                    test.g /= 2;
                    test.b /= 2;
                }
                this->DrawRect(0,0,(608 * prog_b), 456, test);

                test.rgba = 0x000000FF; 
                this->DrawRect(0,0,(608 * prog_c), 456, test);

            }
            else if(this->status == EGG::Fader::FADE_IN) {
                float prog = 1.0f - (this->color.a / 255.0f);
                float prog_a = (prog / 0.6f);
                float prog_b = ((prog - 0.2f) / 0.6f);
                float prog_c = ((prog - 0.4f) / 0.6f);
                test.rgba = 0x78d6bfFF; 
                if(animator){
                    test.rgba = animator->GetCurrentColor();
                }
                this->DrawRect((608 * prog_c),0,608, 456, test);
                test.rgba = 0x40767aFF; 
                if(animator){
                    test.rgba = animator->GetCurrentColor();
                    test.r /= 2;
                    test.g /= 2;
                    test.b /= 2;
                }
                this->DrawRect((608 * prog_b),0,608, 456, test);
                test.rgba = 0x000000FF; 
                this->DrawRect((608 * prog_a),0,608, 456, test);
            }
        }

        void LMUFader::draw(){
            switch(this->type){
                case LMUFader::LMU:
                case LMUFader::LMU_TRACK:
                    this->DrawLMU();
                    return;
                case LMUFader::SWOOP:
                    this->DrawSwoop();
                    return;
                case LMUFader::VANILLA:
                    EGG::ColorFader::draw();
                    return;
            }
            return;
        }

        void LMUFader::LoadSampleTPL() {
            DVDFileInfo info;
            if(!DVDOpen("Scene/UI/Images/173.tpl", &info)) return;
            void* alloc;
            if((Cosmos::Heap::SafeAllocator::Alloc(&alloc, ((info.length + 0x1F) & ~0x1F), 0x20, RKSystem::mInstance.EGGSystem) != Cosmos::Heap::SafeAllocator::SUCCESS)) {
                CosmosLog("Alloc Failed, Free space: %d\n", RKSystem::mInstance.EGGSystem->getAllocatableSize(0x20));
                DVDClose(&info); return;
            }
            CosmosLog("TPL Pointer: %p\n", alloc);
            DVDReadPrio(&info, alloc, info.length, 0, 2);
            this->tpl = (TPLPalettePtr) alloc;
            TPLBind(this->tpl);
            DVDClose(&info); return;
        }

        void InitNewFader(){
            ColorFader* oldFader = RKSystem::mInstance.sceneManager->colorFader;
            testFader = new (RKSystem::mInstance.EGGSystem) LMUFader(oldFader);
            testFader->SetType(LMUFader::VANILLA); 
            testFader->LoadSampleTPL();

            RKSystem::mInstance.sceneManager->colorFader = testFader;
        }
        static BootHook bhInitNewFader(InitNewFader, LOW);

        void ChangeFaderTypeOnBoot(Page* page){
            if(testFader && testFader->getStatus() == EGG::Fader::CAN_FADE_OUT) {
                testFader->SetType(LMUFader::LMU);
            }

        }
        kmWritePointer(0x808bea38, ChangeFaderTypeOnBoot);

        void UpdateFaderStyle(){
            if(!testFader) return;
            MenuId nextMenu = MenuData::GetStaticInstance()->nextMenuId;
            if(nextMenu == POWER_OFF_WII) testFader->SetType(LMUFader::SWOOP);
            else if(Scene::GetType(nextMenu) == CATEGORY_GAMEPLAY && Cosmos::CupManager::GetStaticInstance()->GetWinningTrack() == 0x173) 
                testFader->SetType(LMUFader::LMU_TRACK);
            else testFader->SetType(LMUFader::LMU);
        };
        kmBranch(0x80635afc, UpdateFaderStyle);

    }

}
