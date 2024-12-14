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

#include <System/Sha1.hpp>
#include <core/rvl/os/OS.hpp>

namespace SHA1
{
    u8* GetFileSha1(const void * buffer, u32 size)
    {
        static u8 output[0x14];
        NETSHA1Context context;
        NETSHA1Init(&context);
        NETSHA1Update(&context, buffer, size);
        NETSHA1GetDigest(&context, output);

        return output;
    }
}
