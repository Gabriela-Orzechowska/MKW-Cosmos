#include <kamek.hpp>
#include <game/UI/Page/Menu/TopMenuOverlay.hpp>
#include <game/UI/Page/Other/Title.hpp>

namespace Aurora {
    namespace UI {

        struct AnimationData {
            float frameLength;
            u32 colorCount;
            u32 colorPalette[];
        };

#define PALETTE_LENGTH 4
        class Animator{
        public:
            Animator() : isUsingTop(true), layerAlpha(1.0f) {};
            static void CreateStaticInstance();
            static Animator* GetStaticInstance() { return sInstance; };
            static inline float GetBezierEasing(float a);

            void AnimateTopMenu(Pages::TopMenuOverlay& overlay);
            void ApplyPaneColor(Pane* pane);
            void UpdateColor();
            void SetAnimationData(AnimationData* data) { currentAnimation = data; }

            void AnimateBackground(Pane* pane);
            void UpdateBackground();
            void UpdateLicenseIndex(u32 index);

        private:
            static Animator* sInstance;
            AnimationData* currentAnimation;

            float frame;
            float titleFrameX;
            float titleFrameY;
            u32 currentColor;

            u8 licenseLevel[2];
            float layerAlpha;
            bool isUsingTop;
        };
    }
}
