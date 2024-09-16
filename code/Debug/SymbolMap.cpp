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

#include <Debug/SymbolMap.hpp>
#include <core/System/RKSystem.hpp>
#include <main.hpp>
#include <Debug/IOSDolphin.hpp>



namespace CosmosDebug
{
    SymbolManager * SymbolManager::sInstance = nullptr;

    void SymbolManager::CreateStaticInstance()
    {
        #ifdef DEBUG_COSMOS
        if(IOS::Dolphin::GetVersion() == nullptr) return;
        if(gameID[3] != 'P') return;

        Cosmos::System::Console_Print("[CSD] Loading Symbol Map\n");

        char buffer[0x10] __attribute__((aligned(0x20)));

        DVDFileInfo fileHandle;

        EGG::Heap * heap = RKSystem::mInstance.EGGSystem;
        //EGG::Heap * heap2 = RKSystem::mInstance.EGGRootMEM2;
        SymbolManager * manager = new (heap, 0x20) SymbolManager;

        manager->kamekBaseAddress = *(u32 *)0x80003FFC;

        if(DVDOpen("/Binaries/GameP.BMAP", &fileHandle))
        {
            if(DVDReadPrio(&fileHandle, buffer, 0x10, 0x0, 2))
            {
                memcpy(&manager->header, buffer, 0x10);
                
                u32 nameTableSize = manager->header.filesize - manager->header.nameTableOffset;

                manager->symEntryArray = new (heap, 0x20) SymbolEntry[manager->header.symbolCount]; 
                manager->symNameTable = new (heap, 0x20) char[nameTableSize];
                manager->symName = new (heap, 0x20) char[0x40];

                if(DVDReadPrio(&fileHandle, manager->symEntryArray, (0x8 * manager->header.symbolCount), 0x10, 2))
                {
                    if(DVDReadPrio(&fileHandle, manager->symNameTable, nameTableSize, manager->header.nameTableOffset, 2))
                    {
                        CosmosLog("Symbol map loaded at: %p; %p\n", manager->symEntryArray, manager->symNameTable);
                        SymbolManager::sInstance = manager;
                    }
                }
            }
            DVDClose(&fileHandle);
        }
        else {
            Cosmos::System::Console_Print("[ERR] Map file not found.\n");
        }

        DVDFileInfo fileHandleK;

        if(DVDOpen("/map/KamekM.BMAP", &fileHandleK))
        {
            if(DVDReadPrio(&fileHandleK, buffer, 0x10, 0x0, 2))
            {
                memcpy(&manager->kamekHeader, buffer, 0x10);

                u32 nameTableSize = manager->kamekHeader.filesize - manager->kamekHeader.nameTableOffset;

                manager->kamekSymEntryArray = new (heap, 0x20) KamekSymbolEntry[manager->kamekHeader.symbolCount];
                manager->kamekSymNameTable = new (heap, 0x20) char[nameTableSize];

                if(DVDReadPrio(&fileHandleK, manager->kamekSymEntryArray, (0x8 * manager->kamekHeader.symbolCount), 0x10, 2))
                {
                    if(DVDReadPrio(&fileHandleK, manager->kamekSymNameTable, nameTableSize, manager->kamekHeader.nameTableOffset, 2))
                    {
                        CosmosLog("Kamek Symbol map loaded at: %p; %p\n", manager->kamekSymEntryArray, manager->kamekSymNameTable);
                        CosmosLog("Kamek base address: %08x\n", manager->kamekBaseAddress);
                    }
                }
                DVDClose(&fileHandleK);
            }
        }

        #endif
    }

    void SymbolManager::DestroyStaticInstance()
    {
        if(SymbolManager::sInstance == nullptr) return;

        delete[] SymbolManager::sInstance->symEntryArray;
        delete[] SymbolManager::sInstance->symNameTable;
        delete[] SymbolManager::sInstance->kamekSymEntryArray;
        delete[] SymbolManager::sInstance->kamekSymNameTable;
        delete(SymbolManager::sInstance);

        SymbolManager::sInstance = nullptr;
    }

    char * SymbolManager::GetSymbolName(u32 address)
    {
        SymbolManager * manager = SymbolManager::sInstance;

        if(manager == nullptr) return "<Symbol map not loaded>";

        if(address < 0x80000000 || address > 0x81700000) return "<Invalid address>";

/*
        if(address >= manager->kamekBaseAddress && address < manager->kamekBaseAddress + 0x10000) 
        {
            for(int i = 0; i <manager->kamekHeader.symbolCount-1; i++)
            {
                KamekSymbolEntry entry = manager->kamekSymEntryArray[i];

                u32 offsetAddress = address - manager->kamekBaseAddress;

                if(offsetAddress >= entry.offset && offsetAddress < manager->kamekSymEntryArray[i+1].offset)
                {
                    return manager->kamekSymNameTable + entry.nameOffset;
                }

            }
        }

        KamekSymbolEntry entry = manager->kamekSymEntryArray[manager->kamekHeader.symbolCount-1];
        if((address - manager->kamekBaseAddress) >= entry.offset)
            return manager->kamekSymNameTable + entry.nameOffset;

*/
        for(int i = 0; i < manager->header.symbolCount; i++)
        {
            SymbolEntry entry = manager->symEntryArray[i];
            u32 symAddress = (entry.data1 >> 8) + 0x80000000;
            u16 size = entry.data1 << 8 | entry.data2 >> 24;

            if(address >= symAddress && address < symAddress + size)
            {
                u32 nameOffset = entry.data2 & 0x00FFFFFF;

                return manager->symNameTable + nameOffset;
            }
        }
        return "<Symbol not found>";            
    }

    void InitSymbolMap()
    {
        SymbolManager::CreateStaticInstance();
    }

    static BootHook InitSymbolManager(InitSymbolMap, LOW);

}
