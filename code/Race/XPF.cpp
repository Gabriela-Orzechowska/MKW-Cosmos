#include <Race/XPF.hpp>

namespace Cosmos
{
    XPFManager * XPFManager::sInstance = nullptr;

    void XPFManager::CreateStaticInstance(){
        XPFManager * manager = new XPFManager();
        sInstance = manager;
    }
    static BootHook bhCreateXPFManager(XPFManager::CreateStaticInstance, LOW);

    GOBJ * XPFManager::GetDefinitionObject(u16 id){
        s32 objectCount = KMP::Controller::sInstance->gobj->pointCount;
        id = id & ~0x1000;
        for(int i = 0; i < objectCount; i++)
        {
            GOBJ * gobj = KMP::Controller::sInstance->GetGOBJHolder(i)->raw;
            if(gobj->objID == id) return gobj;
        }
        
        return nullptr;
    }

    void XPFManager::EvaluateConditions(ObjectHolder * holder)
    {
        this->randScenario = random.NextLimited(8);
        CosmosLog("XPF Scenario: %d\n", this->randScenario);

        s32 objectCount = KMP::Controller::sInstance->gobj->pointCount;
        for(int i = 0; i < objectCount; i++)
        {
            GOBJ * gobj = KMP::Controller::sInstance->GetGOBJHolder(i)->raw;
            if(gobj->presenceFlags < 0x1000) continue; //Not a LeCode XPF
            if(gobj->objID >= 0x2000) continue; //We dont calc definition objects

            bool enabled = false;
            if(gobj->padding < 0x2000 & gobj->padding >= 0x1000){
                enabled = CalcPredefinedCondition(gobj->padding);
            }
            if(gobj->padding >= 0x2000){
                enabled = CalcDefinitionObjectCondition(GetDefinitionObject(gobj->padding), (gobj->padding & 0x1000));
            }

            if(!enabled) gobj->presenceFlags = 0x0;
            else gobj->presenceFlags = 0x3f;

            gobj->objID &= 0x3ff;
            
        }
        return;
    }

    bool XPFManager::CalcDefinitionObjectCondition(GOBJ * gobj, bool neg)
    {
        if(gobj == nullptr) return false;

        bool ret = false;

        RacedataScenario * scenario = &RaceData::sInstance->racesScenario;
        DEF_OBJ_MODE mode = DEF_OBJ_BITS;
        if(gobj->objID >= 0x4000) mode = DEF_OBJ_OR;
        if(gobj->objID >= 0x6000) mode = DEF_OBJ_AND;

        if(gobj->padding != 0){
            bool entry = CalcPredefinedCondition(gobj->padding);
            if(!entry) return false;
        }

        switch(mode){
            case DEF_OBJ_BITS:
                GameMode mode = scenario->settings.gamemode;
                if(mode == MODE_BATTLE || mode == MODE_PUBLIC_BATTLE || mode == MODE_PRIVATE_BATTLE){
                    if(scenario->settings.battleType == BATTLE_BALLOON){
                        ret = CalcConditionBits(gobj->settings[0],0);
                    }
                    else{
                        ret = CalcConditionBits(gobj->settings[1],1);
                    }
                }
                else{
                    ret = CalcConditionBits(gobj->settings[2],2);
                }
                // Setting 4 (index 3) contains values for item rain, implement own code for this check
                break;

            case DEF_OBJ_OR:
                for(int i = 0; i < 8; i++){
                    if(gobj->settings[i] == 0) continue;
                    ret = ret || CalcPredefinedCondition(gobj->settings[i]);
                } 
                break;
            case DEF_OBJ_AND:
                ret = true;
                for(int i = 0; i < 8; i++){
                    if(gobj->settings[i] == 0) continue;
                    ret = ret && CalcPredefinedCondition(gobj->settings[i]);
                }
                break;
            default:
                ret = false;
                break;
        }        
        if(neg) ret = !ret;
        return ret;
    }

    bool XPFManager::CalcConditionBits(u16 val, u8 field){
        bool ret = false;
        RacedataScenario * scenario = &RaceData::sInstance->racesScenario;
        u8 localPlayerCount = scenario->localPlayerCount;
        u8 playerCount = scenario->playerCount;

        if(!isOnline()){
            if(val & ((1 << (localPlayerCount-1)))) ret = true;
        }
        else{
            u8 range = playerCount > 6 ? (playerCount-3) / 3 : 0;
            if(val & ((1 << (4 + (range*2) + (localPlayerCount-1))))) ret = true;
        }
        if((val & 0x4000) && field == 2 && scenario->settings.gamemode == MODE_TIME_TRIAL) ret = true; 
        return ret;
    }

    bool XPFManager::CalcPredefinedCondition(u16 val)
    {
        bool ret = false;
        RacedataScenario * scenario = &RaceData::sInstance->racesScenario;
        u8 localPlayerCount = scenario->localPlayerCount;
        u8 playerCount = scenario->playerCount;

        if(val < 0x1e00){
            bool neg = val & 0x1;
            val &= ~0x1;

            switch(val){
                case 0x1000:
                    ret = false; break;
                case 0x1002:
                    ret = localPlayerCount == 1; break;
                case 0x102e:
                    ret = localPlayerCount == 2; break;
                case 0x1030:
                    ret = localPlayerCount == 3; break;
                case 0x1032:
                    ret = localPlayerCount == 4; break;
                case 0x1034:
                    ret = localPlayerCount < 3; break;
                case 0x1036:
                    ret = localPlayerCount & 0x1; break;
                case 0x1038:
                    ret = (localPlayerCount == 1 || localPlayerCount == 4); break;
                // IGNORING everything related to player numbers higher than 12 rn
                case 0x1006:
                    ret = playerCount < 7; break;
                case 0x1008:
                    ret = playerCount < 10; break;
                case 0x100a:
                    ret = true; break; // 1 - 12
                case 0x100e:
                    ret = (playerCount - 7) < 3; break;
                case 0x1010:
                    ret = playerCount > 6; break;
                case 0x1012:
                    ret = playerCount > 6; break;
                case 0x1014:
                    ret = playerCount > 9; break;
                case 0x1016:
                    ret = playerCount > 9; break;
                case 0x1018:
                    ret = false; break; // 13 - 18
                case 0x1004:
                    ret = !isOnline(); break;
                case 0x101a:
                    ret = scenario->settings.gamemode == MODE_TIME_TRIAL; break;
                case 0x101c:
                    ret = scenario->settings.gamemode == MODE_BATTLE || scenario->settings.gamemode == MODE_PRIVATE_BATTLE || scenario->settings.gamemode == MODE_PUBLIC_BATTLE; break;
                case 0x101e:
                    ret = (scenario->settings.gamemode == MODE_BATTLE || scenario->settings.gamemode == MODE_PRIVATE_BATTLE || scenario->settings.gamemode == MODE_PUBLIC_BATTLE) && scenario->settings.battleType == BATTLE_BALLOON; break;
                case 0x1020:
                    ret = (scenario->settings.gamemode == MODE_BATTLE || scenario->settings.gamemode == MODE_PRIVATE_BATTLE || scenario->settings.gamemode == MODE_PUBLIC_BATTLE) &&  scenario->settings.battleType == BATTLE_COIN; break;
                case 0x1022:
                    ret = scenario->settings.gamemode == MODE_VS_RACE || scenario->settings.gamemode == MODE_PRIVATE_VS || scenario->settings.gamemode == MODE_PUBLIC_VS || scenario->settings.gamemode == MODE_TIME_TRIAL; break;
                case 0x1024:
                    ret = false; break; //Item rain, implement your own check
                default:
                    ret = false; break;
            }
            if(neg) ret = !ret;
        }
        //Engine class checks
        else if(val >= 0x1e00 && val < 0x1f00){
            u16 mask = val & 0x7f;
            EngineClass enClass = scenario->settings.engineClass;
            if((val & 0x1) && enClass == CC_BATTLE) ret = true;
            if((val & 0x2) && enClass == CC_50) ret = true;
            if((val & 0x4) && enClass == CC_100) ret = true;
            if((val & 0x8) && enClass == CC_150) ret = true;
            // for 200cc implement your own code
            //if((val & 0x10 && enClass == CC_200)) ret = true;
            if((val & 0x20) && enClass == CC_150 && (scenario->settings.modeFlags & 1)) ret = true;
            //if((val & 0x40) && enClass == CC_200 && (scenario->settings.modeFlags & 1)) ret = true;
        }
        else if(val >= 0x1f00){
            u16 mask = val & 0xff;
            if((mask & (1 << this->randScenario))) ret = true;
        }
        
        return ret;
    }   

    void EvaluateXPF(ObjectHolder * holder, bool isMii){
        XPFManager::GetStaticInstance()->EvaluateConditions(holder);
        holder->CreateAllObjects(isMii);
    }
    kmCall(0x8082a7d4, EvaluateXPF);
} // namespace Cosmos
