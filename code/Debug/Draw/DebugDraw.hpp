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
#include <core/rvl/rvl_sdk.hpp>
#include <core/rvl/gx/GX.hpp>
#include <core/nw4r/g3d/Camera.hpp>
#include <core/egg/Display.hpp>
#include <core/System/RKSystem.hpp>

extern "C"{
    extern u32 sFontData2[77];
    extern u8 asciiTable[0x60]; //802470c8
}


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


        static void Init(){
            // Make the space appear as black square
            asciiTable[0x20] = 0x7a;
            sFontData2[28] = 0x60000000;
            sFontData2[29] = 0x110002a0;
            sFontData2[30] = 0x12a00ae0;
            sFontData2[31] = 0x084012a0;
            sFontData2[32] = 0x10003e1c;
            sFontData2[33] = 0x10001008;
            sFontData2[34] = 0x60000808;
        }

        static DebugMessage* sHook;

        DebugMessage* mNext;
        char message[0x60];
        bool isDisplayed;
        bool hasTimer;
        int curTime;

    };

    void DrawRect(s16 x, s16 y, s16 width, s16 height, GXColor color);

} // namespace CosmosDebug
