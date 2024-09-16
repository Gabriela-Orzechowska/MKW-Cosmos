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
#include <game/System/Archive.hpp>
#include <game/Objects/ObjectHolder.hpp>
#include <main.hpp>


static bool hasHighwayManager = false;
// TODO WE SHOULD CHECK IF WE CAN MAKE AS IT ONLY HAS LIMITED SPACE: EG PUSHY PENGUINS CRASHES BECAUSE OF THIS
kmWrite32(0x8082a4fc,0x48000030); //Always create control group

#pragma region MoonviewHighway

void * HighwayManagerCreate(u32 size)
{
    bool hasCar = false;
    bool hasTruck = false;
    u32 carCount = ObjectHolder::GetStaticInstance()->GetControlledCount();
    for(int i = 0; i < carCount; i++)
    {
        Object * object = ObjectHolder::GetStaticInstance()->GetControlledObjectByID(i);
        const char * objectName = object->GetName();
        if(strcmp(objectName, "car_body") == 0x0)
            hasCar = true;
        else if(strcmp(objectName, "kart_truck") == 0x0)
            hasTruck = true;
    }

    if(hasCar && hasTruck)
    {
        hasHighwayManager = true;
        return EGG::Heap::alloc(size, 4, 0);
    }

   return NULL;
}
//Noop MH Check (Make Slot Independent)
kmWrite32(0x808279a4, 0x60000000); 

kmCall(0x808279ac, HighwayManagerCreate);
kmWrite32(0x808279b4, 0x41820018);

u32 HighwayManagerCheck()
{
    return (u32) hasHighwayManager;
}

kmCall(0x80827f3c, HighwayManagerCheck);
kmWrite32(0x80827f40, 0x2c030000);

kmCall(0x806d686c, HighwayManagerCheck);
kmWrite32(0x806d6870, 0x2c030000);

kmWrite32(0x8078df44, 0x48000070); //Mat Fix

#pragma endregion

void * HeyhoShipManagerCreate(u32 size)
{
    u32 controlledCount = ObjectHolder::GetStaticInstance()->GetControlledCount();
    for(int i = 0; i < controlledCount; i++)
    {
        Object * object = ObjectHolder::GetStaticInstance()->GetControlledObjectByID(i);
        const char * objectName = object->GetName();
        if(strcmp(objectName, "HeyhoShipGBA") == 0x0)
        {
            return EGG::Heap::alloc(size, 4, 0);
        }
    }
    return NULL;
}
kmCall(0x80827a34, HeyhoShipManagerCreate);
kmWrite32(0x80827a3c, 0x41820018);

kmWrite32(0x80827a2c, 0x60000000);


void * LoadAdditionalBinaries(ArchiveRoot& archive, ArchiveSource source, const char * name)
{
    void * file = archive.GetFile(ARCHIVE_HOLDER_COURSE, name, 0);
    if(file == NULL)
    {
        char commonName[0x30];
        snprintf(commonName, 0x30, "Common/%s", name);
        file = archive.GetFile(ARCHIVE_HOLDER_COURSE, commonName, 0);
    }
    if(file == NULL)
        file = archive.GetFile(ARCHIVE_HOLDER_COMMON, name, 0);
    return file;
}

kmCall(0x8082c140, LoadAdditionalBinaries);
kmCall(0x807f92ac, LoadAdditionalBinaries);
