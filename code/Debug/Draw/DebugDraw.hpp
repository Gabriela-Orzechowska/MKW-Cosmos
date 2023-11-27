#pragma once

#include <kamek.hpp>
#include <core/rvl/gx/GX.hpp>
#include <core/nw4r/g3d/Camera.hpp>
#include <core/egg/Display.hpp>
#include <core/System/RKSystem.hpp>

namespace DXDebug
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
    void DrawRect(s16 x, s16 y, s16 width, s16 height, GXColor color);

} // namespace DXDebug
