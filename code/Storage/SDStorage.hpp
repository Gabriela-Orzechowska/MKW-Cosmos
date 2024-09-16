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
#include <main.hpp>

class SDStorage : public StorageDevice
{
    public:
    static bool Init();

    u32 diskSectorSize() override;
    bool diskRead(u32 sector, u32 count, void * buffer) override;
    bool diskWrite(u32 sector, u32 count, const void * buffer) override;
    bool diskErase(u32 sector, u32 count) override;
    bool diskSync() override;
    u32 diskGetMessageId() override;
};
