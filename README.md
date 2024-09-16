![image](https://gabriela-orzechowska.com/images/cosmosLogo.png)

Mario Kart Wii code extension for CT distributions.

## Building
Run `build.py`. Update `CC=` path inside `build.py` if needed.

### Requires
[CodeWarrior for MPC55xx/MPC56xx v2.10 Special Edition](https://nxp.com/lgfiles/devsuites/PowerPC/CW55xx_v2_10_SE.exe) ([mirror](https://cache.nxp.com/lgfiles/devsuites/PowerPC/CW55xx_v2_10_SE.exe))

## Current Features
### General
- Slot Expansion
- Discord Rich Presence
- Files loading from additional archives/bmg files
- Auto BRSAR Patching
- Full ghost support (saving, watching, racing)
- Various debugging features
- SD Card Support
- LZMA Compression
- Custom Track Selection
- Performance Monitor

### Gameplay
- Time Trial CC mode selection
- No multi channel music cut off
- Speed-o-meter
- Extended Presence Flags
- Many minor fixes and patches
  
### Settings
Settings Page, containing various settings, available also mid-race:
- Race
  - Music Speed-Up
  - Draggable Blues
  - FPS Mode
  - Toggle Mii Heads
  - Speedometer Settings
- Menu
  - Language Selector (All vanilla + ability to add more)
  - Track Layout chooser (basic vs alphabetical)
also can be changed with minus on track selection)
  - Fast Menus
- Debug (during developement)
  - DWC Logs
  - Performance Monitor
  - Debug Messages on Screen
  - Log Console to SD Card
- Host
  - OpenHost
  - Race Count
  - HAW
  - Allow Mii Heads
  - Force CC
- Accesibility (menu to be added)
  - Y/ZL to Wheelie

## License

    Cosmos
    Copyright (C) 2023-2024 Gabriela Orzechowska

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


## Credits
- Documentation by Melg: https://github.com/MelgMKW/Mario-Kart-Wii-Kamek-Documentation/tree/main
- FatFs: http://elm-chan.org/fsw/ff/
- 7-zip for LZMA: https://sourceforge.net/projects/sevenzip/files/7-Zip/
