#include <kamek.hpp>
#include <game/System/Archive.hpp>
#include <game/Objects/ObjectHolder.hpp>
#include <main.hpp>

static bool hasHighwayManager = false;

kmWrite32(0x8082a4fc,0x48000030); //Always create control group

#pragma region MoonviewHighway

void * HighwayManagerCreate(u32 size)
{
    bool hasCar = false;
    bool hasTruck = false;
    u32 carCount = ObjectHolder::sInstance->GetControlledCount();
    for(int i = 0; i < carCount; i++)
    {
        Object * object = ObjectHolder::sInstance->GetControlledObjectByID(i);
        const char * objectName = object->GetName();
        if(strcmp(objectName, "car_body") == 0x0)
        {
            hasCar = true;
        }
        if(strcmp(objectName, "kart_truck") == 0x0)
        {
            hasTruck = true;
        }
    }

    if(hasCar && hasTruck)
    {
        hasHighwayManager = true;
        return new u8[size];
    }

   return NULL;
}
//Noop MH Check (Make Slot Independent)
kmWrite32(0x808279a4, 0x60000000); 

kmCall(0x808279ac, HighwayManagerCreate);
kmWrite32(0x808279b4, 0x41820018);

u32 HighwayManagerCheck()
{
    if(hasHighwayManager) return 0x1;
    else return 0x0;
}

kmCall(0x80827f3c, HighwayManagerCheck);
kmWrite32(0x80827f40, 0x2c030000);

kmCall(0x806d686c, HighwayManagerCheck);
kmWrite32(0x806d6870, 0x2c030000);

kmWrite32(0x8078df44, 0x48000070); //Mat Fix

#pragma endregion

void * HeyhoShipManagerCreate(u32 size)
{
    u32 controlledCount = ObjectHolder::sInstance->GetControlledCount();
    for(int i = 0; i < controlledCount; i++)
    {
        Object * object = ObjectHolder::sInstance->GetControlledObjectByID(i);
        const char * objectName = object->GetName();
        if(strcmp(objectName, "HeyhoShipGBA") == 0x0)
        {
            return new u8[size];
        }
    }
    return NULL;
}
kmCall(0x80827a34, HeyhoShipManagerCreate);
kmWrite32(0x80827a3c, 0x41820018);

kmWrite32(0x80827a2c, 0x60000000);


void * LoadAdditionalBinaries(ArchiveRoot * archive, ArchiveSource source, const char * name)
{
    void * file = archive->GetFile(ARCHIVE_HOLDER_COURSE, name, 0);
    if(file == NULL)
    {
        char commonName[0x30];
        snprintf(commonName, 0x30, "Common/%s", name);
        file = archive->GetFile(ARCHIVE_HOLDER_COURSE, commonName, 0);
    }
    if(file == NULL)
        file = archive->GetFile(ARCHIVE_HOLDER_COMMON, name, 0);
    return file;
}

kmCall(0x8082c140, LoadAdditionalBinaries);
kmCall(0x807f92ac, LoadAdditionalBinaries);
