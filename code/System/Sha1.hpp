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

#include <kamek.hpp>

typedef struct {
    u8 unknown[0x60];
} NETSHA1Context;

extern void NETSHA1Init(NETSHA1Context *context);
extern void NETSHA1Update(NETSHA1Context *context, const void *input, u32 length);
extern void NETSHA1GetDigest(NETSHA1Context *context, void *digest);

namespace SHA1
{
    u8* GetFileSha1(const void * buffer, u32 size);
}

