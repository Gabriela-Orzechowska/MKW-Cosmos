#pragma once

#include <kamek.hpp>
#include <game/Objects/ObjectHolder.hpp> 
#include <game/System/Random.hpp>
#include <game/KMP/Controller.hpp>
#include <game/Race/RaceData.hpp>



namespace Cosmos
{  

    bool isOnline() {
        GameMode mode = RaceData::sInstance->racesScenario.settings.gamemode;
        if(mode <= MODE_6) return false;
        if(mode >= MODE_AWARD) return false;
        return true;
    }

    bool isTT() {
        GameMode mode = RaceData::sInstance->racesScenario.settings.gamemode;
        if(mode != MODE_TIME_TRIAL && mode != MODE_GHOST_RACE) return false;
        return true;
    }

    bool isBattle() {
        GameMode mode = RaceData::sInstance->racesScenario.settings.gamemode;
        if(mode != MODE_BATTLE && mode != MODE_PRIVATE_BATTLE && mode != MODE_PUBLIC_BATTLE) return false;
        return true;
    }
    bool isVS() {
        GameMode mode = RaceData::sInstance->racesScenario.settings.gamemode;
        if(mode != MODE_GRAND_PRIX && mode != MODE_VS_RACE && mode != MODE_TIME_TRIAL && mode != MODE_PUBLIC_VS
        && mode != MODE_PRIVATE_VS) return false;
        return true;
    }

    enum DEF_OBJ_MODE{
        DEF_OBJ_BITS = 0x0,
        DEF_OBJ_OR = 0x1,
        DEF_OBJ_AND = 0x2,
    };

    class XPFManager {
        public:
        static XPFManager * GetStaticInstance() {return sInstance;} 
        static void CreateStaticInstance();

        void EvaluateConditions(ObjectHolder& holder);

        private:
        GOBJ * GetDefinitionObject(u16 id) const;
        bool CalcDefinitionObjectCondition(GOBJ * gobj, bool neg);
        bool CalcPredefinedCondition(u16 value);
        bool CalcConditionBits(u16 val, u8 field);
        

        static XPFManager * sInstance;
        Random random;
        s8 randScenario;
    };
} // namespace Cosmos
