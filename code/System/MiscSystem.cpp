/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
