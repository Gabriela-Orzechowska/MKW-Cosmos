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
    char * GetFileSha1(const void * buffer, u32 size)
    {
        static u8 output[0x14];
        static char stringOutput[0x29];
        NETSHA1Context context;
        NETSHA1Init(&context);
        NETSHA1Update(&context, buffer, size);
        NETSHA1GetDigest(&context, output);

        for(int i = 0; i < 0x14; i++)
        {
            u8 n1 = output[i] >> 4;
            u8 n2 = output[i] & 0x0F;
            stringOutput[i*2] = n1 < 0xA ? '0' + n1 : 'a' + n1 - 0xA;
            stringOutput[i*2+1] = n2 < 0xA ? '0' + n2 : 'a' + n2 - 0xA;
        }
        stringOutput[0x28] = '\0';
        return stringOutput;
    }
}
