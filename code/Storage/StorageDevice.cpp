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

#include <Storage/StorageDevice.hpp>
#include <core/rvl/os/OS.hpp>

StorageDevice * StorageDevice::currentDevice = nullptr;

StorageDevice * StorageDevice::GetDevice()
{
    return currentDevice;
}

u32 StorageDevice_SectorSize()
{
    if(StorageDevice::currentDevice == nullptr) return -1;
    return StorageDevice::currentDevice->diskSectorSize();
}

bool StorageDevice_Read(u32 sector, u32 count, void * buffer)
{
    if(StorageDevice::currentDevice == nullptr) {
        return false;
    }
    return StorageDevice::currentDevice->diskRead(sector,count,buffer);
}

bool StorageDevice_Write(u32 sector, u32 count, const void * buffer)
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskWrite(sector,count,buffer);
}

bool StorageDevice_Erase(u32 sector, u32 count)
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskErase(sector,count);
}

bool StorageDevice_Sync()
{
    if(StorageDevice::currentDevice == nullptr) return false;
    return StorageDevice::currentDevice->diskSync();
}

u32 StorageDevice_GetMessageId()
{
    if(StorageDevice::currentDevice == nullptr) return -1;
    return StorageDevice::currentDevice->diskGetMessageId();
}
