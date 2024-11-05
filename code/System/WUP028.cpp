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
 * to use this code in your projects without the requirement covering your
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

#include "Input/InputData.hpp"
#include "core/rvl/ipc/ipc.hpp"
#include "core/rvl/os/OS.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include "types.hpp"
#include <System/WUP028.hpp>
#include <core/rvl/vi.hpp>

namespace Cosmos {

    WUP028Manager* WUP028Manager::sInstance = nullptr;

    void WUP028Manager::OnInit(){
        mLastDataWrite = OSGetTime(); 
        mIsStarted = true;
        s32 ret = IOS::Open("/dev/usb/hid", IOS::MODE_NONE);
        if(ret < 0) {
            mIsWorking = false;
            return;
        }

        mHidFd = ret;
        s32 ret4 = IOS::IOCtl(mHidFd, IOS::IOCTL_HID4_GET_VERSION, 
                nullptr, 0, nullptr, 0);

        if(ret4 == 0x40001) {
            mHidVersion = 4;
            OnInitVer4();
        }
        else {
            s32 ret5 = IOS::IOCtl(mHidFd, IOS::IOCTL_HID5_GET_VERSION, 
                    nullptr, 0, nullptr, 0);
            if(ret5 == 0x50001) { 
                mHidVersion = 5;
                //OnInitVer5();
            }
            else mHidVersion = -1;
        }
    }

    void WUP028Manager::CustomPADRead(PADStatus* status){
        if(!mIsStarted) return;
        if(mIsWorking && mIsInit) {
            if(OSTicksToMilliseconds(OSGetTime() - mLastDataWrite) > GCN_TIMEOUT_MS){
                for(int i = 0; i < 4; i++){
                    mStatus[i].error = -1;
                }
            }
            for(int i = 0; i < 4; i++){
                {
                    const u32 compareVal = 3;
                    if(
                        abs(status[i].sliderL - mStatus[i].sliderL) > compareVal ||
                        abs(status[i].sliderR - mStatus[i].sliderR) > compareVal ||
                        abs(status[i].verticalStickU8 - mStatus[i].verticalStickU8) > compareVal ||
                        abs(status[i].horizontalStickU8 - mStatus[i].horizontalStickU8) > compareVal ||
                        abs(status[i].verticalCStickU8 - mStatus[i].verticalCStickU8) > compareVal ||
                        abs(status[i].horizontalCStickU8 - mStatus[i].horizontalCStickU8) > compareVal ||
                        status[i].buttons != mStatus[i].buttons) {
                        VIResetSIIdle();     
                    }
                }

                status[i] = mStatus[i];
            }
        }
        else {
            PADRead(status);
        }
    }

    void PatchPADRead(PADStatus* status){
        if(!!WUP028Manager::GetStaticInstance()){
            WUP028Manager::GetStaticInstance()->CustomPADRead(status);
        }
    }   
    kmCall(0x80523910, PatchPADRead);
    kmCall(0x805237c4, PatchPADRead);
    kmCall(0x80217464, PatchPADRead);
    kmCall(0x80007880, PatchPADRead);

    static u8 InitMsgBuffer[1] __attribute__((aligned(0x20))) = { CMD_INIT,};
    static u8 PollMsgBuffer[(POLL_SIZE + 0x1f) & ~0x1f] __attribute__((aligned(0x20)));
    static u8 RumbleMsgBuffer[5] __attribute__((aligned(0x20))) = { CMD_RUMBLE, };
    static InterruptMsg4 InitMsg4 __attribute__((aligned(0x20))) = {
        .device = -1,
        .endpoint = ENDPOINT_OUT,
        .size = sizeof(InitMsgBuffer),
        .ptr = InitMsgBuffer,
    };
    static InterruptMsg4 PollMsg4 __attribute__((aligned(0x20))) = {
        .device = -1,
        .endpoint = ENDPOINT_IN,
        .size = POLL_SIZE,
        .ptr = PollMsgBuffer,
    };
    static InterruptMsg4 RumbleMsg4 __attribute__((aligned(0x20))) = {
        .device = -1,
        .endpoint = ENDPOINT_OUT,
        .size = sizeof(RumbleMsgBuffer),
        .ptr = RumbleMsgBuffer,
    };

    void WUP028Manager::OnUsbPoll(s32 ret){
        if(ret >= 0){
            mIsWorking = true;
            if(*PollMsgBuffer == 0x21){
                s32 isr = OSDisableInterrupts();
                for(int i = 0; i < 4; i++){
                    u8* data = PollMsgBuffer + (i*9 + 1);
                    u8 status = data[0] >> 4;
                    if(status != 1 && status != 2){
                        mStatus[i].error = -1;
                        continue;
                    }
                    u16 buttonData = ((data[1] >> 0) & 1 ? GCN_A : 0);
                    buttonData |= ((data[1] >> 1) & 1 ? GCN_B : 0);
                    buttonData |= ((data[1] >> 2) & 1 ? GCN_X : 0);
                    buttonData |= ((data[1] >> 3) & 1 ? GCN_Y : 0);
                    buttonData |= ((data[1] >> 4) & 1 ? GCN_DPAD_LEFT : 0);
                    buttonData |= ((data[1] >> 5) & 1 ? GCN_DPAD_RIGHT : 0);
                    buttonData |= ((data[1] >> 6) & 1 ? GCN_DPAD_DOWN : 0);
                    buttonData |= ((data[1] >> 7) & 1 ? GCN_DPAD_UP : 0);
                    buttonData |= ((data[2] >> 0) & 1 ? GCN_START : 0);
                    buttonData |= ((data[2] >> 1) & 1 ? GCN_Z : 0);
                    buttonData |= (data[7] >= GCN_TRIGGER_THRESHOLD ? GCN_L : 0);
                    buttonData |= (data[8] >= GCN_TRIGGER_THRESHOLD ? GCN_R : 0);

                    PADStatus& gcn = mStatus[i];
                    gcn.buttons = buttonData;
                    gcn.horizontalStickU8 = data[3] - 128;
                    gcn.verticalStickU8 = data[4] - 128;
                    gcn.horizontalCStickU8 = data[5] - 128;
                    gcn.verticalCStickU8 = data[6] - 128;
                    gcn.sliderL = data[7];
                    gcn.sliderR = data[8];
                    gcn.unknown[0] = 0;
                    gcn.unknown[1] = 0;
                    gcn.error = 0;
                }
                mLastDataWrite = OSGetTime();
                OSRestoreInterrupts(isr);
            }
            DCFlushRange(PollMsgBuffer, sizeof(PollMsgBuffer));
            ret = IOS::IOCtlAsync(mHidFd, IOS::IOCTL_HID4_INTERRUPT_IN,
                &PollMsg4, sizeof(PollMsg4), nullptr, 0,
                WUP028Manager::OnUsbPollCallback, nullptr);
        }   
        if(ret){
            OnError();
        }
    }

    void WUP028Manager::OnInitVer4(){
        CosmosLog("Initing version 4 usb\n");
        s32 ret = IOS::IOCtlAsync(mHidFd, IOS::IOCTL_HID4_GET_DEVICE_CHANGE,
                nullptr, 0, &this->mDeviceChangeSizeBuffer, 0x600,
                WUP028Manager::OnUsbChangeVer4Callback, nullptr);
    }

    void WUP028Manager::OnUsbChangeVer4(s32 hid){
        CosmosLog("Hid ret: %d\n", hid);

        CosmosLog("Data buffer: %p\n", mDeviceChangeSizeBuffer);
        if(hid >= 0){
            bool found = false;
            for(int i = 0; i < 0x180 && mDeviceChangeSizeBuffer[i] < 0x600; i += mDeviceChangeSizeBuffer[i]/4){
                if(mDeviceChangeSizeBuffer[i] == USB_DESCRIPTOR_SIZE && 
                        mDeviceChangeSizeBuffer[i + 4] == DEVICE_ID){
                    found = true;
                    CosmosLog("Detected USB Adapter\n");
                    CosmosLog("Initting adapter\n"); 
                    u32 deviceId = mDeviceChangeSizeBuffer[i+1];
                    if(mAdapterId != deviceId){
                        mAdapterId = deviceId;
                        InitMsg4.device = mAdapterId;
                        (void)IOS::IOCtlAsync(mHidFd, IOS::IOCTL_HID4_INTERRUPT_OUT,
                                &InitMsg4, sizeof(InitMsg4), nullptr, 0,
                                WUP028Manager::OnUsbInitVer4Callback, nullptr);
                    }
                    break;
                }
                if(!found) {
                    mAdapterId = -1U;
                    mIsInit = true;
                    mIsWorking = false;
                }
                hid = IOS::IOCtlAsync(mHidFd, IOS::IOCTL_HID4_GET_DEVICE_CHANGE,
                        nullptr, 0, this->mDeviceChangeSizeBuffer, 0x600,
                        WUP028Manager::OnUsbChangeVer4Callback, nullptr);
            }
        }
        if(hid) {
            OnError();
        }
    }

    void WUP028Manager::OnUsbInitVer4(s32 ret){
        if(ret >= 0){
            //TODO ADD RUMBLE
            CosmosLog("Setting up poll callback\n");
            mIsInit = true;
            PollMsg4.device = mAdapterId;
            DCFlushRange(PollMsgBuffer, sizeof(PollMsgBuffer));
            ret = IOS::IOCtlAsync(mHidFd, IOS::IOCTL_HID4_INTERRUPT_IN,
                &PollMsg4, sizeof(PollMsg4), nullptr, 0,
                WUP028Manager::OnUsbPollCallback, nullptr);
        }
        else{
            CosmosLog("Failed to init adapter: %d\n", ret); 
            OnError();
        }
    }

    void CreateWUPInstance(){
        WUP028Manager::CreateStaticInstance();
    }
    static BootHook bhCreateWUP(CreateWUPInstance, LOW);
}
