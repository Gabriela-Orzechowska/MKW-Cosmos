#include "System/Identifiers.hpp"
#include <UI/Multiplayer/FriendRoom.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Page/Other/YesNo.hpp>

void PatchOpenRoom(Pages::FriendMenu& menu, PushButton* button, u32 hudSlotId) {
    Scene* scene = MenuData::GetStaticInstance()->GetCurrentScene();
    Pages::YesNoPopUp* yesNo = scene->Get<Pages::YesNoPopUp>(VOTERANDOM_MESSAGE_BOX);
    yesNo->SetMessageBoxMsg(0x2806, nullptr);
    yesNo->SetupButton(0, 0x7d2, nullptr, 0, nullptr);
    yesNo->SetupButton(1, 0x7d2, nullptr, 0, nullptr);
}
