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
#include <Debug/IOSDolphin.hpp>

class RichPresenceManager
{
    public:
    static RichPresenceManager * sInstance;
    
    static RichPresenceManager * GetStaticInstance() { return sInstance; }
    static void CreateStaticInstance();
    static s32 InitConnection();

    static IOS::Dolphin::DiscordRichPresence presence;

    void Init();
    void CloseConnection();
    s32 UpdateStatus();

    private:
    bool rEnabled;
};
