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

#include <kamek.hpp>
#include <game/UI/Text.hpp>
#include <game/UI/Layout/Layout.hpp>
#include <core/rvl/os/OS.hpp>
#include <Settings/UserData.hpp>
#include <core/System/SystemManager.hpp>


wchar_t* GetMessageFromAdditionalHolder(u32 bmg);
void GetTextFromMessage(char * out, u32 messageId);
