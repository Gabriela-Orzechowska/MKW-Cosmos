#include <game/System/SaveDataManager.hpp>
#include <game/UI/MenuData/MenuData98.hpp>
#include <game/UI/Ctrl/PushButton.hpp>
#include <kamek.hpp>

void SetFavouriteCombo(GlobalContext& context, PushButton& button) {
    SaveDataManager* manager = SaveDataManager::GetStaticInstance();
    LicenseManager& license = manager->GetLicense(button.buttonId - 1);

    context.ChangeLicense();
    context.characters[0] = license.GetFavouriteCharacter();
    context.karts[0] = license.GetFavouriteKart();
}
kmCall(0x805eb7c4, SetFavouriteCombo);
