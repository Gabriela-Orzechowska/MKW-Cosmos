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
#include <core/rvl/dvd/dvd.hpp>
#include <core/rvl/os/OS.hpp>
#include <core/egg/Heap.hpp>

namespace CosmosDebug
{
    typedef struct SymbolMapHeader
    {
        u32 magic;
        u32 filesize;
        u32 symbolCount;
        u32 nameTableOffset;
    } SymbolMapHeader;

    typedef struct SymbolEntry
    {
        u32 data1;
        u32 data2;
    } SymbolEntry;
 
    typedef struct KamekSymbolEntry
    {
        u32 offset;
        u32 nameOffset;
    } KamekSymbolEntry;

    class SymbolManager{
        public:
            static void CreateStaticInstance();
            static void DestroyStaticInstance();
            static char * GetSymbolName(u32 address);

            static SymbolManager * sInstance;
            u32 kamekBaseAddress;
            SymbolMapHeader header;
            SymbolMapHeader kamekHeader;
            SymbolEntry * symEntryArray; //Array
            KamekSymbolEntry * kamekSymEntryArray; //Array
            char * symName;
            char * symNameTable;
            char * kamekSymNameTable;
        private:
            bool isFromDVD;
            
    };
}
