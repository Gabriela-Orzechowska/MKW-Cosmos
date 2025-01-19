/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2025 Gabriela Orzechowska
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
 * The following code is based on the libogc SD interface implementation.
 * https://github.com/devkitPro/libogc/blob/master/libogc/wiisd.c
 * 
 * The original license goes as follows:
 * 
 Copyright (c) 2008
   Michael Wiedenbauer (shagkur)
   Dave Murphy (WinterMute)
   Sven Peter <svpe@gmx.net>
	
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <Storage/SDStorage.hpp>
#include <Storage/disc/SD.hpp>
#include <vendor/ff/ff.h>
#include <core/System/RKSystem.hpp>
#include <Debug/Draw/DebugDraw.hpp>

u32 SDStorage::diskSectorSize()
{
    return SD_SectorSize();
}

bool SDStorage::diskRead(u32 sector, u32 count, void * buffer)
{
    return SD_Read(sector, count, buffer);
}

bool SDStorage::diskWrite(u32 sector, u32 count, const void * buffer)
{
    return SD_Write(sector,count,buffer);
}

bool SDStorage::diskErase(u32 sector, u32 count)
{
    return SD_Erase();
}

bool SDStorage::diskSync()
{
    return true;
}

u32 SDStorage::diskGetMessageId()
{
    return SD_GetMessageId();
}

void QuickFatal(char * string)
{
    u32 black = 0;
    u32 white = ~0;
    OSFatal(&white, &black, string);
}

const char sd_fd[] __attribute__((aligned(0x20))) = "/dev/sdio/slot0";

/*
 * This method is based on original libogc one as well as the MKW-SP implementation 
 * of SD Card interface
 * https://github.com/mkw-sp/mkw-sp/blob/main/payload/sp/storage/Sdi.c
 * 
 * The original license text goes as follows:
 * 
 * Copyright 2021-2023 Pablo Stebler
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

bool SDStorage::Init()
{
    s32 fd;

    if(sdfd < 0){
        fd = Cosmos::Open((char *) sd_fd, IOS::MODE_NONE);
    }
    else fd = sdfd;
    Cosmos::System::Console_Print("[SDi] Opening SD interface\n");
    if(fd < 0)
    {
        Cosmos::System::Console_Print("[ERR] Failed to open SD Card\n");
        CosmosLog("Failed to open /dev/sdio/slot0");
        return false;
    }

    CosmosLog("Opened SD interface, ID: %i\n", fd);

    if(!SD_Reset(fd)) {
        CosmosLog("Failed to reset SD Card\n");
        Cosmos::System::Console_Print("[ERR] Failed to reset SD Card\n");
        return false;
    }
    u32 status;
    if(!SD_GetStatus(&status)) 
    {
        CosmosLog("Unable to get status\n");
        Cosmos::System::Console_Print("[ERR] Unable to get status\n");
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INSERTED)){
        CosmosLog("SD card not inserted\n");
        Cosmos::System::Console_Print("[ERR] SD card not inserted\n");
        return false;
    }

    if(!(status & SDIO_STATUS_CARD_INITIALIZED))
    {
        CosmosLog("Could not initialize filesystem... Retrying\n");
        Cosmos::System::Console_Print("[WAR] Could not initialize SD\n");
        bool ret = SD_Reinitialize();
        SD_GetStatus(&status);

        if(!(status & SDIO_STATUS_CARD_INITIALIZED) || !ret){
            CosmosLog("Retry initialization has failed...");
            Cosmos::System::Console_Print("\n[ERR] Retry initialization has failed...\n");
            return false;
        } 
        CosmosLog("Success\n");
        Cosmos::System::Console_Print("[SDi] Resetted SD Card successfully\n");
    }

    //sdhc = !!(status & SDIO_STATUS_CARD_SDHC);
    if(!SD_Enable4bitBus()){
        CosmosLog("Failed to enable 4-bit mode\n");
        return false;
    }

    if(!SD_SetClock(1))
    {
        CosmosLog("Unable to set the clock\n");
        return false;
    }

    if(!SD_Select())
    {
        CosmosLog("Unable to select the sd card\n");
        return false;
    }

    if(!SD_SetCardBlockSize(SECTOR_SIZE)){
        SD_Deselect();
        CosmosLog("Unable to set block size\n");
        return false;
    }

    if(!SD_EnableCard4BitBus()){
        SD_Deselect();
        CosmosLog("Unable to enable card 4 bit bus\n");
        return false;
    }

    SD_Deselect();
    StorageDevice * device = new(RKSystem::mInstance.EGGSystem) SDStorage;
    StorageDevice::currentDevice = device;

    CosmosLog("Successfully initialized SD card\n");
    Cosmos::System::Console_Print("[SDi] Successfully initialized SD card\n");
    
    FRESULT result = f_mount(&device->m_fs, L"", 1);
    
    if(result != FR_OK)
    {
        char strbuffer[0x40];
        CosmosLog("Couldn't initialize FAT\n");
        Cosmos::System::Console_Print("[ERR] Couldn't initialize FAT\n");
        StorageDevice::currentDevice = nullptr;
        return false;
    }
    CosmosLog("Mounted FAT\n");
    Cosmos::System::Console_Print("[SDi] Mounted FAT filesystem\n");

    return true;
}
