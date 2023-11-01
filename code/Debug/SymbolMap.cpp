#include <Debug/SymbolMap.hpp>
#include <core/System/RKSystem.hpp>
#include <main.hpp>

#define DEBUG_DX

namespace DXDebug
{
    SymbolManager * SymbolManager::sInstance = nullptr;

    void SymbolManager::CreateStaticInstance()
    {
        #ifdef DEBUG_DX
        char buffer[0x10] __attribute__((aligned(0x20)));

        DVDFileInfo fileHandle;

        EGG::Heap * heap = RKSystem::mInstance.EGGSystem;
        SymbolManager * manager = new (heap, 0x20) SymbolManager;

        if(DVDOpen("/map/GameP.BMAP", &fileHandle))
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
                        OSReport("[DX] Symbol map loaded at: %p", manager->symEntryArray);
                        SymbolManager::sInstance = manager;
                    }
                }
            }
            DVDClose(&fileHandle);
        }

        #endif
    }

    void SymbolManager::DestroyStaticInstance()
    {
        if(SymbolManager::sInstance == nullptr) return;

        delete[] SymbolManager::sInstance->symEntryArray;
        delete(SymbolManager::sInstance);

        SymbolManager::sInstance = nullptr;
    }

    char * SymbolManager::GetSymbolName(u32 address)
    {
        SymbolManager * manager = SymbolManager::sInstance;

        if(manager == nullptr) return "";

        for(int i = 0; i < manager->header.symbolCount; i++)
        {
            SymbolEntry entry = manager->symEntryArray[i];
            u32 symAddress = (entry.data1 >> 8) + 0x80000000;
            u16 size = entry.data1 << 8 | entry.data2 >> 24;

            if(address >= symAddress && address < symAddress + size)
            {
                u32 nameOffset = entry.data2 & 0x00FFFFFF;

                DVDFileInfo fileHandle;

                return manager->symNameTable + nameOffset;
            }
        }
        return "<Symbol not found>\n";            
    }

    void InitSymbolMap()
    {
        SymbolManager::CreateStaticInstance();
    }

    static BootHook InitSymbolManager(InitSymbolMap, LOW);

}