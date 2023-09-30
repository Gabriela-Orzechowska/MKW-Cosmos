#ifndef _KARTPOINTERS_
#define _KARTPOINTERS_
#include <kamek.hpp>
#include <game/Input/InputData.hpp>
#include <game/Race/Kart/KartCollision.hpp>
#include <game/Race/Kart/KartDamage.hpp>
#include <game/Race/Kart/KartPart.hpp>
#include <game/Race/Kart/KartMovement.hpp>
#include <game/Race/Kart/KartParams.hpp>
#include <game/Race/Kart/KartPhysics.hpp>
#include <game/Race/Kart/KartStatus.hpp>
#include <game/Race/Kart/KartSub.hpp>
#include <game/Race/Kart/Suspensions.hpp>
#include <game/Race/Kart/WheelPhysics.hpp>
#include <game/Visual/Model/ModelDirector.hpp>
#include <game/Sound/Actors/CharacterSound.hpp>
#include <game/Sound/Actors/KartSound.hpp>
#include <game/System/Camera.hpp>
#include <game/Objects/KartObject.hpp>
#include <game/Collision/Entity/EntityHolder.hpp>
#include <game/Visual/ClipInfoMgr.hpp>

class GFX44;
class KartPointers {
public:

  void LinkKartList(); //80590138
  KartValues *values; //0x0
  KartStatus *kartStatus; //0x4
  KartBody *kartBody;
  KartSuspensions **suspensions;
  Wheels **wheels; //0x10
  KartModel *model;
  KartSub *kartSub;
  KartSound *kartSound;
  int *shadow; //0x20
  UnkCameraStruct *camera_0x24;
  KartMovement *kartMovement; //0x28
  KartDamage *kartDamage; //0x2c
  KartCollision *kartCollision;
  KartObjectCollision *kartObject;
  int *lakituStruct; //0x38
  u8 unknown_0x3C[8];
  int *lights_0x44;
  int *unknown_0x48;
  Entity *entity; //0x4C
  ClipInfo *clipInfo; //0x50
  int *unknown_0x54[(0x60-0x54)/4];
  int *bulletBillModel; //0x60
}; //Total size 0x64
static_assert(sizeof(KartPointers) == 0x64, "KartPointers");
#endif