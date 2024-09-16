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

#ifndef _STORAGEDEVICEH_
#define _STORAGEDEVICEH_

typedef unsigned int u32;
typedef int bool;

u32 StorageDevice_SectorSize__Fv();
bool StorageDevice_Read__FUiUiPv(u32 sector, u32 count, void * buffer);
bool StorageDevice_Write__FUiUiPCv(u32 sector, u32 count, const void * buffer);
bool StorageDevice_Erase__FUiUi(u32 sector, u32 count);
bool StorageDevice_Sync__Fv();
u32 StorageDevice_GetMessageId();

#endif
