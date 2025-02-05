#include "UI/Text.hpp"
#include "core/nw4r/snd/SoundArchivePlayer.hpp"
#include "hooks.hpp"
#include <kamek.hpp>
#include <Settings/UserData.hpp>
#include <game/UI/Ctrl/UIControl.hpp>

namespace Aurora {
    namespace Sound {
        using namespace Cosmos::Data;

        void SetVolumeText(LayoutUIControl* control, u32 option) {
            if(control == nullptr) return;

            TextInfo info;
            info.intToPass[0] = option;
            control->SetMsgId(0xd7b, &info);
        }

        void InitSoundSettings(){
            GetDefinitions()[AURORA_SOUND_SETTINGS].settings[0].setTextFunc = SetVolumeText;
            GetDefinitions()[AURORA_SOUND_SETTINGS].settings[1].setTextFunc = SetVolumeText;
            GetDefinitions()[AURORA_SOUND_SETTINGS].settings[2].setTextFunc = SetVolumeText;
        }
        static BootHook bhInitSoundSettings(InitSoundSettings, LOW);

        void SoundArchivePlayerNewUpdate(snd::SoundArchivePlayer& player){
            SettingsHolder* holder = SettingsHolder::GetStaticInstance();
            float mainVolume = (float)holder->GetSettingValue(AURORA_SETTING_SOUND_MAIN);
            float musicVolume = mainVolume * holder->GetSettingValue(AURORA_SETTING_SOUND_MUSIC) / 100.0f;
            float gameVolume = mainVolume * holder->GetSettingValue(AURORA_SETTING_SOUND_GAME) / 100.0f;

            float oldVolumes[20];

            oldVolumes[0] = player.soundPlayerArray[0].mainOutVolume;
            player.soundPlayerArray[0].mainOutVolume *= musicVolume;
            for(int i = 1; i < player.soundPlayerCount && i < 20; i++) {
                oldVolumes[i] = player.soundPlayerArray[i].mainOutVolume;
                player.soundPlayerArray[i].mainOutVolume *= gameVolume;
            }

            player.Update();

            player.soundPlayerArray[0].mainOutVolume = oldVolumes[0];
            for(int i = 0; i < player.soundPlayerCount && i < 20; i++){
                player.soundPlayerArray[i].mainOutVolume = oldVolumes[i];
            }
        }

        kmBranch(0x80210fe4, SoundArchivePlayerNewUpdate);

    }
}
