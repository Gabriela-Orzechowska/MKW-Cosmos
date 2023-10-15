#include <kamek.hpp>
#include <game/System/Archive.hpp>
#include <game/Objects/ObjetHolder.hpp>

static bool hasHighwayManager = false;

void * TGECarManagerCreate(u32 size)
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
        return new u8[size];
        hasHighwayManager = true;
    }

   return NULL;
}
//Noop MH Check (Make Slot Independent)
kmWrite32(0x808279a4, 0x60000000); 

kmCall(0x808279ac, TGECarManagerCreate);
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

