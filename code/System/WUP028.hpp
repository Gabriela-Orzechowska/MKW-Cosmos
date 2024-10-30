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
 *
 * As a special exception, the copyright holders of Cosmos give you permission 
 * to us this code with other software without the requirement covering your
 * work by the GNU General Public License. All of the other requirements still apply.
 *
 * The GNU General Public License gives permission to release a modified version 
 * without this exception; this exception also makes it possible to release a modified 
 * version which carries forward this exception. 
 * This exception is an additional permission under section 7 of the 
 * GNU General Public License, version 3 (“GPLv3”)
 * 
 * This code is based on Alexandra "Chadderz" Chadwick implementation of the module
 * for "BrainSlug".
 *
 * https://github.com/Chadderz121/wup-028-bslug
 *
 * The original license goes as follows:
 *
 *
 *  Copyright (C) 2017, Alex Chadwick
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once
#include "Input/InputData.hpp"
#include <kamek.hpp>
#include <core/System/RKSystem.hpp>
#include <core/rvl/os/OS.hpp>
#include <core/rvl/rvl_sdk.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/wpad.h>

namespace Cosmos {
    const u32 GCN_CONTROLLER_COUNT = 4;
    const u32 GCN_TRIGGER_THRESHOLD = 170;
    const u32 GCN_TIMEOUT_MS = 1500;
   
    const u32 USB_DESCRIPTOR_SIZE = 0x44;

    enum WUP_VAL {
        CMD_INIT = 0x13,
        CMD_RUMBLE = 0x11,
        POLL_SIZE = 0x25,
        DEVICE_ID = 0x057e0337,
        ENDPOINT_OUT = 0x2,
        ENDPOINT_IN = 0x81,

    };

    struct InterruptMsg4 {
        u32 padding[4];
        u32 device;
        u32 endpoint;
        u32 size;
        void* ptr;
    };

    class WUP028Manager{
    public:
        WUP028Manager() : mIsStarted(false), mIsWorking(false), mIsInit(false), mAdapterId(-1U) {}
        static void CreateStaticInstance() {
            sInstance = new (RKSystem::mInstance.EGGSystem, 0x40) WUP028Manager;
            if(sInstance) {
                sInstance->OnInit();
            }
        }
        static WUP028Manager* GetStaticInstance() { return sInstance; }

        void OnInit();
        void CustomPADRead(PADStatus* status);

        void OnError() {
            mIsWorking = false;
//            IOS::Close(mHidFd);
            mHidFd = -1U;
        }

        static void OnUsbPollCallback(s32 ret, void* arg){
            sInstance->OnUsbPoll(ret);
        }
        void OnUsbPoll(s32 ret);

        // Version 4
        void OnInitVer4();
        static void OnUsbChangeVer4Callback(s32 hid, void* arg) { 
            sInstance->OnUsbChangeVer4(hid);
        }
        void OnUsbChangeVer4(s32 hid);

        static void OnUsbInitVer4Callback(s32 ret, void* arg) {
            sInstance->OnUsbInitVer4(ret);
        }
        void OnUsbInitVer4(s32 ret);


    protected:
        bool mIsStarted;
        bool mIsWorking;
        bool mIsInit;
        u8 mHidVersion;
        OSTime mLastDataWrite; 
        u32 mHidFd;
        u32 mAdapterId;

        u32 mDeviceChangeSizeBuffer[0x180] __attribute__((aligned(0x20)));
        u32 mTmpBufferSize[0x20] __attribute__((aligned(0x20)));
        PADStatus mStatus[4];

        static WUP028Manager* sInstance;
    };
}
