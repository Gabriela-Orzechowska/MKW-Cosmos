#include <game/System/SaveDataManager.hpp>
#include <game/UI/MenuData/MenuData98.hpp>
#include <game/UI/Ctrl/PushButton.hpp>
#include <kamek.hpp>

void SetFavouriteCombo(GlobalContext& context, PushButton& button) {
    LicenseManager& license = SaveDataManager::GetStaticInstance()->GetLicense(button.buttonId - 1);

    context.Reset();
    context.characters[0] = license.GetFavouriteCharacter();
    context.karts[0] = license.GetFavouriteKart();
}
kmCall(0x805eb7c4, SetFavouriteCombo);
