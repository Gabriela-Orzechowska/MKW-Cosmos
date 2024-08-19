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


## Credits
- Documentation by Melg: https://github.com/MelgMKW/Mario-Kart-Wii-Kamek-Documentation/tree/main
- FatFs: http://elm-chan.org/fsw/ff/
- 7-zip for LZMA: https://sourceforge.net/projects/sevenzip/files/7-Zip/
