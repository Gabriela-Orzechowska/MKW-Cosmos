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
#include <core/rvl/os/mutex.hpp>
#include <vendor/ff/ff.h>

class StorageDevice
{
    public:
    static StorageDevice * GetDevice();
    static StorageDevice * currentDevice;

    virtual u32 diskSectorSize() = 0;
    virtual bool diskRead(u32 sector, u32 count, void * buffer) = 0;
    virtual bool diskWrite(u32 sector, u32 count, const void * buffer) = 0;
    virtual bool diskErase(u32 sector, u32 count) = 0;
    virtual bool diskSync() = 0;
    virtual u32 diskGetMessageId() = 0;

    OSMutex m_mutex;
    FATFS m_fs;
    bool m_ok;
};
