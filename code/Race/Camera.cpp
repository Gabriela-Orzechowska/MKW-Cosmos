#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/Race/RaceData.hpp>
#include <Controller/MiscController.hpp>

static Vec3 cameraPos;
#define TARGET_DISTANCE 2000.0f
#define ROTATION_MULT 2.0f
#define MOVEMENT_SPEED 40.0f
static float fi = 0.0f;
static float theta = 0.0f;

void CustomCamera(KartMovement * movement)
{
    KartBase * base = &movement->base;
    RaceCamera * camera = base->pointers->raceCamera;

    u32 controller = MenuData::sInstance->pad.padInfos[0].controllerSlotAndTypeActive;
    ControllerType type = ControllerType(controller & 0xFF);
    RealControllerHolder * holder = &InputData::sInstance->realControllerHolders[0];

    bool isPressed = false;
    u32 raw = (holder->inputStates[0].buttonRaw & ~holder->inputStates[1].buttonRaw);
    if(type == GCN)
    {
        if(raw & GCN_Y) isPressed = true;
    }
    else if(type == CLASSIC)
    {
        if(raw & CLASSIC_Y) isPressed = true;
    }

    if(isPressed)
    {
        if(camera->flags & 0x800)
        {
            camera->flags = 0;
            camera->enableFovEffects = 1;
        }
        else
        {
            camera->flags |= 0xFFFF;
            camera->enableFovEffects = 0;

            Vec3 relative;

            relative.x = camera->target.x - camera->position.x;
            relative.y = camera->target.y - camera->position.y;
            relative.z = camera->target.z - camera->position.z;
            
            //float distance = PSVECMag(&relative);
            //theta = atan(relative.z/relative.x) / 0.01745329f;
            //fi = asin(relative.y/distance) / 0.01745329f;

            cameraPos = camera->position;
        }
    }
    movement->UpdateWheelieBump();
}

kmCall(0x805792b0, CustomCamera);

//80791910
void UpdateCameraPos(RaceCamera * camera)
{
    if(camera->flags & 0x800)
    {
        u32 controller = MenuData::sInstance->pad.padInfos[0].controllerSlotAndTypeActive;
        ControllerType type = ControllerType(controller & 0xFF);
        RealControllerHolder * holder = &InputData::sInstance->realControllerHolders[0];
        u32 raw = (holder->inputStates[0].buttonRaw);

        float stickX = holder->inputStates[0].stickX;
        float stickY = holder->inputStates[0].stickY;

        float rStickX = 0.0f;
        float rStickY = 0.0f;
        bool isUpPressed = false;
        bool isDownPressed = false;
        bool isSpeedPressed = false;

        if(type == GCN)
        {
            rStickX = ((GCNController *)holder->controller)->cStickHorizontal;
            rStickY = ((GCNController *)holder->controller)->cStickVertical;
            isUpPressed = (raw & GCN_R);
            isDownPressed = (raw & GCN_L);
            isSpeedPressed = (raw & GCN_Z);
        }
        else if(type == CLASSIC)
        {
            rStickX = ((WiiController *)holder->controller)->kpadStatus->cStickHorizontal;
            rStickY = ((WiiController *)holder->controller)->kpadStatus->cStickVertical;
            isUpPressed = (raw & CLASSIC_R);
            isDownPressed = (raw & CLASSIC_L);
            isSpeedPressed = (raw & CLASSIC_ZR);
        }

        theta += rStickY * ROTATION_MULT;
        fi += rStickX * ROTATION_MULT;

        if(theta <= -180.0f) theta = -180.0f;
        if(theta >= 180.0f) theta = 180.0f;

        if(fi > 360.0f) fi -= 360.0f;
        if(fi < -360.0f) fi += 360.0f;

        float fi_rad = fi * 0.01745329f;
        float theta_rad = theta * 0.001745329f * 4;

        float speed = MOVEMENT_SPEED;
        if(isSpeedPressed) speed *= 3;

        float forwardspeed = speed * stickY;
        float sidewaysSpeed = speed * stickX;

        Vec3 movementPos;
        movementPos.x = forwardspeed * cos(theta_rad) * cos(fi_rad);
        movementPos.z = forwardspeed * cos(theta_rad) * sin(fi_rad);
        movementPos.y = forwardspeed * sin(theta_rad);

        movementPos.x += sidewaysSpeed * cos(theta_rad) * cos(fi_rad + (3.14f/2));
        movementPos.z += sidewaysSpeed * cos(theta_rad) * sin(fi_rad + (3.14f/2));

        cameraPos.x += movementPos.x;
        cameraPos.y += movementPos.y;
        cameraPos.z += movementPos.z;


        if(isDownPressed)
            cameraPos.y -= speed;
        if(isUpPressed)
            cameraPos.y += speed;

        Vec3 targetPos;

        targetPos.x = TARGET_DISTANCE * cos(theta_rad) * cos(fi_rad);
        targetPos.z = TARGET_DISTANCE * cos(theta_rad) * sin(fi_rad);
        targetPos.y = TARGET_DISTANCE * sin(theta_rad);

        PSVECAdd(&targetPos, &cameraPos, &targetPos);

        camera->position = cameraPos;
        camera->target = targetPos;
        camera->up.x = 0.0f;
        camera->up.y = 1.0f;
        camera->up.z = 0.0f;
    }
    camera->DoUpdateMatrix();
}
kmCall(0x805a2acc, UpdateCameraPos);

void StopPlayerInput(KartStatus * status)
{
    RaceCamera * camera = status->base->pointers->raceCamera;
    if(camera->flags & 0x800)
    {
        status->stickX = 0.0f;
        status->stickY = 0.0f;
        status->bitfield0 &= ~BITFIELD0::ACCELERATE;
        status->bitfield0 &= ~BITFIELD0::BRAKE;
    }
    else status->UpdateFromInput();
}

kmCall(0x805967a4, StopPlayerInput);


struct ItemHolderPlayer
{
    KartPointers * pointers;
};
extern void ItemHolderPlayerb4_use(ItemHolderPlayer * param1, u32 param2);

void StopItem(ItemHolderPlayer * param_1, u32 param_2)
{
    if(param_1->pointers->raceCamera->flags & 0x800) return;
    else ItemHolderPlayerb4_use(param_1, param_2);
}

kmCall(0x80797f94, StopItem);