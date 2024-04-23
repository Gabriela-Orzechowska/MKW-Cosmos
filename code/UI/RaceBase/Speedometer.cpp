#include <UI/RaceBase/Speedometer.hpp>

namespace CosmosUI{

    u32 ControlRaceSpeedometer::Count()
    {
        memset(speedos, 0, sizeof(speedos));

        u32 result = RaceData::GetStaticInstance()->racesScenario.localPlayerCount;
        MenuId menuId = MenuData::GetStaticInstance()->curScene->menuId;

        if(menuId >= WATCH_GHOST_FROM_CHANNEL && menuId <= WATCH_GHOST_FROM_MENU) result++;

        return result;
    }
    

    void ControlRaceSpeedometer::Create(Page * page, u32 index)
    {
        u32 screens = Count();

        u8 type = (screens == 3) ? 4 : screens;
        for(int i = 0; i < screens; i++)
        {
            ControlRaceSpeedometer * speedometer = new(ControlRaceSpeedometer);
            page->AddControl(index + i, speedometer, 0);
            char speedoVariant[0x30];
            snprintf(speedoVariant, 0x30, "CtrlRaceSpeedo_%1d_%1d", type, i);
            speedos[i] = speedometer;
            speedometer->Load(speedoVariant, i);
        }
    }
    
    static CustomControlBuilder SpeedOMeter(ControlRaceSpeedometer::Count, ControlRaceSpeedometer::Create);

    void ControlRaceSpeedometer::Load(const char * variant, u8 id)
    {
        this->hudSlotId = id;

        ControlLoader loader(this);

        const char* anims[] = {"eHundreds", "texture_pattern_0_9_0", NULL,
        "eTens", "texture_pattern_0_9_1", NULL,
        "eUnits", "texture_pattern_0_9_2", NULL,
        "eDot", "texture_pattern_0_9_3",NULL,
        "eDecimals", "texture_pattern_0_9_4",NULL, 
        "eDecimals2", "texture_pattern_0_9_5",NULL,
        NULL};

        loader.Load("game_image", "speedometer", variant, anims);
        
        for(int i = 0; i < 6; i++)
        {
            this->animator.GetAnimationGroupById(i)->PlayAnimationAtFrameAndDisable(0, 0.0f);
        }

        u8 speedosetting = CosmosData::SettingsHolder::GetInstance()->GetSettingValue(CosmosData::COSMOS_SETTING_SPEEDOMETER);
        this->SetSpeedoVariant(speedosetting);
        return;
    }

    void ControlRaceSpeedometer::SetSpeedoVariant(u8 variant){
        this->places[0] = -1; this->places[1] = -1; this->places[2] = -1; this->places[3] = -1; this->places[4] = -1;

        this->isHidden = false;
        switch(variant){
            case CosmosData::SPEEDO_DISABLED:
                this->isHidden = true;
                break;
            case CosmosData::SPEEDO_0_DIGIT:
                this->places[0] = 3; this->places[1] = 4; this->places[2] = 5;
                this->animator.GetAnimationGroupById(0)->PlayAnimationAtFrameAndDisable(0, 10.0f);
                this->animator.GetAnimationGroupById(1)->PlayAnimationAtFrameAndDisable(0, 10.0f);
                this->animator.GetAnimationGroupById(2)->PlayAnimationAtFrameAndDisable(0, 10.0f);
                break;
            case CosmosData::SPEEDO_1_DIGIT:
                this->places[0] = 1; this->places[1] = 2; this->places[2] = 3; this->places[4] = 5;
                this->animator.GetAnimationGroupById(4)->PlayAnimationAtFrameAndDisable(0, 11.0f);
                this->animator.GetAnimationGroupById(0)->PlayAnimationAtFrameAndDisable(0, 10.0f);
                break;
            case CosmosData::SPEEDO_2_DIGIT:
                this->places[0] = 0; this->places[1] = 1; this->places[2] = 2; this->places[3] = 4; this->places[4] = 5;
                this->animator.GetAnimationGroupById(3)->PlayAnimationAtFrameAndDisable(0, 11.0f);
                break;

        }
    }

    void ControlRaceSpeedometer::Init()
    {
        this->HudSlotColorEnable("kmh", true);
        this->HudSlotColorEnable("speed0", true);
        this->HudSlotColorEnable("speed1", true);
        this->HudSlotColorEnable("speed2", true);
        this->HudSlotColorEnable("speed3", true);
        this->HudSlotColorEnable("speed4", true);
        this->HudSlotColorEnable("speed5", true);

        LayoutUIControl::Init();
        return;
    }

    void ControlRaceSpeedometer::OnUpdate()
    {
        this->UpdatePausePosition();

        u8 playerId = this->GetPlayerId();

        KartPointers* pointers = &KartHolder::GetStaticInstance()->GetKart(playerId)->pointers;
        KartPhysics* kartPhysics = pointers->kartBody->kartPhysicsHolder->kartPhysics;

        float maxSpeed = pointers->kartMovement->hardSpeedLimit;

        //We add engine speed + moving road speed + moving water speed;
        Vec3 vec3Speed;
        PSVECAdd(&kartPhysics->engineSpeed, &kartPhysics->movingRoadSpeed, &vec3Speed);
        PSVECAdd(&vec3Speed, &kartPhysics->waterStreamSpeed, &vec3Speed);
        float speed = PSVECMag(&vec3Speed);

        if(speed > maxSpeed) speed = maxSpeed;

        u32 splitSpeed = (u32) (speed * 100.0f);

        float decimal2 = (float )(splitSpeed % 10 / 1);
        float decimal = (float) (splitSpeed % 100 / 10);
        float digit_1 = (float) (splitSpeed % 1000 / 100);
        float digit_2 = (float) (splitSpeed % 10000 / 1000);
        float digit_3 = (float) (splitSpeed % 100000 / 10000);

        if(digit_3 < 0.1f) 
        {
            digit_3 = 10.0f;
            if(digit_2 < 0.1f) 
                digit_2 = 10.0f;
        }

        float numbers[] = { digit_3, digit_2, digit_1, decimal, decimal2 };
        for(int i = 0; i < 5; i++){
            if(this->places[i] == -1) continue;
            this->animator.GetAnimationGroupById(this->places[i])->PlayAnimationAtFrameAndDisable(0, numbers[i]);
        }
        return;
    }


    void UpdateSpeedoDigits(){
        u8 variant = CosmosData::SettingsHolder::GetInstance()->GetSettingValue(CosmosData::COSMOS_SETTING_SPEEDOMETER);

        for(int i = 0; i < 4; i++){
            if(speedos[i] == nullptr) break;
            speedos[i]->SetSpeedoVariant(variant);
        }

    }

    static SettingsValueUpdateHook svuhSpeedo(UpdateSpeedoDigits, CosmosData::COSMOS_SETTING_SPEEDOMETER);

}