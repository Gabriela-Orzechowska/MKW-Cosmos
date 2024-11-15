#include "Settings/UserData.hpp"
#include "UI/Page/Other/Title.hpp"
#include "hooks.hpp"
#include "kamek.hpp"
#include <Aurora/UIAnimation.hpp>
#include <include/c_string.h>

extern "C" u16 FuchiColor1[16]; //80895cc2
extern "C" u16 FuchiColor2[16]; //80895cca


namespace Aurora {
    namespace UI {
        AnimationData rainbow = {
            3.0f,
            4,
            {0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0xFF0000FF},
        };
        AnimationData aurora = {
            3.0f,
            6,
            {0x14e81eff, 0x00ea8dff, 0x0e93f0ff, 0xb53dffff, 0x8d00c4ff,0x14e81eff},
        };
        AnimationData white = {
            1.0f,
            2,
            {-1U, -1U},
        };


        Animator* Animator::sInstance = nullptr;

        void Animator::CreateStaticInstance(){
            sInstance = new Animator;
            sInstance->currentAnimation = &aurora;
        }
        static BootHook bhCreateAnimator(Animator::CreateStaticInstance, LOW);

        void Animator::ApplyPaneColor(Pane* pane){
            if(strstr(pane->name, "_ag") != nullptr){
                Material* mat = pane->GetMaterial();
                if(mat){
                    mat->tevKColours[0] = currentColor;
                }       
            }
            for(PaneList::Iterator it = pane->childList.GetBeginIter(); it != pane->childList.GetEndIter(); ++it){
                this->ApplyPaneColor(&*it);
            }

        }

        float Animator::GetBezierEasing(float a){
            return (a * a * (3.0f - 2.0f * a));
        }

        void Animator::UpdateColor(){
            this->frame += 1/59.94f;
            AnimationData* data = this->currentAnimation;
            float frameLength = data->frameLength;
            float totalTime = data->frameLength * (data->colorCount - 1);

            while(this->frame > totalTime) this->frame -= totalTime;

            int index = 0;
            float prog = this->frame;
            while(prog > data->frameLength) {
                index++;
                prog -= data->frameLength;
            }

            float c = this->GetBezierEasing(prog / data->frameLength);
            u8* color1 = (u8*)&data->colorPalette[index];
            u8* color2 = (u8*)&data->colorPalette[index+1];
            u8 endColor[4];

            for(int i = 0; i < 4; i++){
                endColor[i] = color1[i] * (1-c) + color2[i] * c;
                FuchiColor2[i] = endColor[i];
                FuchiColor1[i] = endColor[i] / 1.1f;
            }
            
            this->currentColor = *(u32*)&endColor;
        }

        void Animator::AnimateTopMenu(Pages::TopMenuOverlay& overlay){
            this->UpdateColor();
            for(int i = 0; i < 2; i++){
                LayoutUIControl* control = (LayoutUIControl*) overlay.controlGroup.controlArray[i];
                if(control == nullptr) return;
                Pane* rootPane = control->layout.layout.rootPane;
                if(rootPane == nullptr) return;

                this->ApplyPaneColor(rootPane);
            }
        }

        void Animator::AnimateBackground(Pane* pane){
            const float speedX = 8.0f;
            const float speedY = 3.0f;

            if(pane != nullptr){
                float width = pane->size.x / 5.0f;
                float height = pane->size.z / 4.0f;

                this->titleFrameX += (1/59.94f) * speedX;
                this->titleFrameY += (1/59.94f) * speedY;
                if(this->titleFrameX > width) this->titleFrameX -= width;
                if(this->titleFrameY > height) this->titleFrameY -= height;
                pane->trans.x = this->titleFrameX;
                pane->trans.y = this->titleFrameY;
            }
        }

        void TopMenuAnimation(Pages::TopMenuOverlay& overlay){
            Animator::GetStaticInstance()->AnimateTopMenu(overlay);
        }
        kmWritePointer(0x808d9e90, TopMenuAnimation);

        void RaceFrameUIUpdate(){
            Animator::GetStaticInstance()->UpdateColor();
        }   
        kmBranch(0x80554f64, RaceFrameUIUpdate);
        

        void OnThemeSettingUpdate(){
            u8 value = Cosmos::Data::SettingsHolder::GetStaticInstance()->GetSettingValue(Cosmos::Data::AURORA_SETTING_MENU_THEME);
            AnimationData* data = &aurora;
            if(value == Cosmos::Data::THEME_RAINBOW) data = &rainbow;
            if(value == Cosmos::Data::THEME_WHITE) data = &white;
            Animator::GetStaticInstance()->SetAnimationData(data);
            Animator::GetStaticInstance()->UpdateColor();
        }
        static SettingsValueUpdateHook svuhThemeSettingUpdate(OnThemeSettingUpdate, Cosmos::Data::AURORA_SETTING_MENU_THEME);
        static MenuLoadHook mlhThemeUpdate(OnThemeSettingUpdate);

        void UpdateTitle(Pages::Title& title){
            Pane* pane = title.titleImage.layout.GetPaneByName("title_cosmos");
            Animator::GetStaticInstance()->AnimateBackground(pane);
        }
        kmWritePointer(0x808beef0, UpdateTitle);
        void UpdateBlurryTitle(Pages::BlurryTitle& title){
            Pane* pane = title.blurryTitleImage.layout.GetPaneByName("chara");
            Animator::GetStaticInstance()->AnimateBackground(pane);
        }
        kmWritePointer(0x808bee8c, UpdateBlurryTitle);

    }
}
