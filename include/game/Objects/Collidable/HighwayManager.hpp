#ifndef _HIGHWAY_MANAGER_
#define _HIGHWAY_MANAGER_
#include <kamek.hpp>
#include <game/Objects/Collidable/GeoObjectCollidable.hpp>

class HighwayManager : public GeoObjectCollidable
{
    explicit HighwayManager(); //806d2908
    ~HighwayManager() override;
    void Setup() override;
    void Update() override;
    bool HasLod() override; 
    void LoadModels() override;
    void LoadCollision() override; //0x60
    void LoadRoute() override; //0x64 80820980

    
};


#endif