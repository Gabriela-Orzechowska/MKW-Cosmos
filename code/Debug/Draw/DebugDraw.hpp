#pragma once

#include <kamek.hpp>
#include <core/rvl/gx/GX.hpp>
#include <core/nw4r/g3d/Camera.hpp>
#include <core/egg/Display.hpp>
#include <core/System/RKSystem.hpp>

namespace nw4r
{
    // NW4R CharWriter
    class TextureFilter
    {
    public:
        GXTexFilter         atSmall;
        GXTexFilter         atLarge;

        bool                operator !=(const TextureFilter& rhs) const
        {
            return (atSmall != rhs.atSmall)
                || (atLarge != rhs.atLarge);
        }
    };


    class LoadingTexture
    {
    public:
        GXTexMapID          slot;
        const void*         texture;
        TextureFilter       filter;

        bool                operator !=(const LoadingTexture& rhs) const
        {
            return (slot != rhs.slot)
                || (texture != rhs.texture)
                || (filter != rhs.filter);
        }
        void                Reset()
        {
            slot    = GX_TEXMAP_NULL;
            texture = NULL;
        }
    };
} // namespace nw4r


namespace CosmosDebug
{   
    class DebugDrawHook {
    private:
        typedef void (Func)();
        Func *mFunc;
        DebugDrawHook * mNext;

        static DebugDrawHook * sHooks;

    public:
        DebugDrawHook(Func * f) {
            mNext = sHooks;
            sHooks = this;
            mFunc = f;
        }

        static void exec() {
            for (DebugDrawHook * p = sHooks; p; p = p->mNext)
                p->mFunc();
        }
    };

    class DebugMessage{
    public:
        DebugMessage(bool displayed) : isDisplayed(displayed) {
            mNext = sHook;
            sHook = this;
            memset(&this->message, 0, 0x60);
        };

        DebugMessage(bool displayed, const char* initialMsg) : isDisplayed(displayed) {
            mNext = sHook;
            sHook = this;
            int size = strlen(message)+1;
            strcpy(message, initialMsg);
        };

        void SetMessage(const char* next){
            int size = strlen(next)+1;
            if(size > 0x60) return;
            strcpy(message,next);
        }
        void DisplayForX(int x){
            this->curTime = x;
            this->hasTimer = true;
        }
        void Display() {
            this->hasTimer = false;
            this->isDisplayed = true;
        }
        void Hide() {
            this->hasTimer = false;
            this->isDisplayed = false;
        }

        int Print(int x, int y);

        static void RenderAll();



        static DebugMessage* sHook;

        DebugMessage* mNext;
        char message[0x60];
        bool isDisplayed;
        bool hasTimer;
        int curTime;
        

    };

    void DrawRect(s16 x, s16 y, s16 width, s16 height, GXColor color);

    extern "C"{
        extern u32 sFontData2[77];
    }

} // namespace CosmosDebug
