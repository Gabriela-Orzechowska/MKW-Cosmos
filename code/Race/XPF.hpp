#pragma once

#include <kamek.hpp>
#include <game/Objects/ObjectHolder.hpp> 
#include <game/System/Random.hpp>
#include <game/KMP/Controller.hpp>
#include <game/Race/RaceData.hpp>



namespace Cosmos
{  
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
