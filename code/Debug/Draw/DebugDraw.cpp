#include <Debug/Draw/DebugDraw.hpp>
#include <Debug/Draw/PerformanceMonitor.hpp>
#include <core/nw4r/ut/RomFont.hpp>
#include <core/rvl/gx/GX.hpp>
#include <core/rvl/gx/GXTexture.hpp>
#include <core/rvl/vi.hpp>
#include <core/nw4r/db/Exception.hpp>
#include <Settings/UserData.hpp>

extern void AsyncDisplay_endRender(EGG::AsyncDisplay * display);
extern void AsyncDisplay_beginRender(EGG::AsyncDisplay * display);
extern void AsyncDisplay_beginFrame(EGG::AsyncDisplay * display); //8020fe24
extern OSTime AsyncDisplay_getTickPerFrame(EGG::AsyncDisplay * display); //8020fe24


namespace CosmosDebug
{
    DebugDrawHook * DebugDrawHook::sHooks = NULL;
    DebugMessage* DebugMessage::sHook = nullptr;

    void DrawRect(s16 x, s16 y, s16 width, s16 height, GXColor color)
    {
        GXSetChanMatColor(GX_COLOR0A0, color);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        {
            GX_Position2s16(x,y);
            GX_Position2s16(x + width,y);
            GX_Position2s16(x + width,y + height);
            GX_Position2s16(x,y + height);
        }
        GXEnd();
    }

    void AsyncDisplayInjectCustomRender(EGG::AsyncDisplay * display)
    {
        GXSetViewport(0.0f,0.0f,608.0f,456.0f,0.0f,1.0f);
        GXSetScissor(0,0, 608, 456);
        float mtx[4][4];
        C_MTXOrtho(mtx, 0.0f, 456.0f, 0.0f, 608.0f, 0.0f, -1.0f);
        GXSetProjection(mtx, GX_ORTHOGRAPHIC);
        float posMtx[3][4];
        PSMTXIdentity(posMtx);
        GXLoadPosMtxImm(posMtx,0);
        GXSetCurrentMtx(0);

        GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
        GXSetNumChans(1);
        GXSetNumIndStages(0);
        GXSetNumTexGens(0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEX_DISABLE, GX_COLOR0A0);
        GXSetCullMode(GX_CULL_BACK);
        GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
        GXSetNumTevStages(1);
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVDSTALPHA, GX_LO_COPY);
        GXSetColorUpdate(GX_TRUE);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);

        GXClearVtxDesc();
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_S16, 0);

        DebugMessage::RenderAll();

        DebugDrawHook::exec();
        

        AsyncDisplay_endRender(display);

        return;
    }

    kmWritePointer(0x802a26f0, AsyncDisplayInjectCustomRender);

    static DebugMessage GeckoDisplay(false, "Dolphin Cheats Enabled.\nErrors might occur");

    void DisplayTestMsg(){
        if((*(u32*)0x80001804) == 1)
            GeckoDisplay.DisplayForX(240);
    }
    static BootHook bhDisplayTestMsg(DisplayTestMsg, FIRST);

    void DebugMessage::RenderAll(){
        CosmosData::SettingsHolder* holder = CosmosData::SettingsHolder::GetInstance();

        if(!holder) return;

        if(holder->GetSettingValue(CosmosData::COSMOS_SETTING_DEBUG_MSGS) == CosmosData::DISABLED) return;

        bool display = false;
        for (DebugMessage * p = sHook; p; p = p->mNext){
            if(p->isDisplayed || p->hasTimer) {
                display = true;
                break;
            }
        }
        if(!display) return;

        void* buffer = VIGetNextFrameBuffer();

        nw4r::db::DirectPrint_ChangeXfb(buffer, 608, 456);

        int x = 0;
        int y = 215;
        int z = 0;
        for (DebugMessage * p = sHook; p; p = p->mNext){
            y -= p->Print(x,y) * 10;
        }
            

        nw4r::db::DirectPrint_StoreCache();

    }

    int DebugMessage::Print(int x, int y) {
        if(!isDisplayed && !hasTimer) return 0;
        int lineCount = 1;
        
        for(char* msg = message; *msg; msg++){
            if(*msg == '\n') lineCount++;
        }
        y -= (lineCount - 1) * 10;
        nw4r::db::DirectPrint_DrawString(x,y,1,message);

        if(curTime > 0) curTime -= 1;
        else this->hasTimer = false;
        return lineCount;
    }
}