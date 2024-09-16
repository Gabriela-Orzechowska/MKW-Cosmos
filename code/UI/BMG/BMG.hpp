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

#ifndef _COSMOS_BMG_SET_
#define _COSMOS_BMG_SET_

enum COSMOS_BMG
{
    BMG_ENABLED_DISABLED = 0x37000,
    BMG_SETTINGS_PAGE = 0x2949,
    BMG_SETTINGS_PAGE_BOTTOM = 0x2a21,

    BMG_SETTINGS_TITLE = 0x2a00,
    BMG_SETTING_CATEGORY = 0x2a01,

    BMG_SETTING_FIELD = 0x30000, //
    BMG_SETTING_OPTION = 0x30001, //

    BMG_SETTING_OPTION_BOTTOM = 0x40001,

    BMG_CUPS = 0x6000,

    BMG_TRACKS = 0x7000,
    BMG_GROUPS = 0x7000,
    BMG_VARIANTS = 0x210000,
};


#endif
