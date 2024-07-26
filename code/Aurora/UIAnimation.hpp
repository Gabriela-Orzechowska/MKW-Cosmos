#include <kamek.hpp>
#include <game/UI/Page/Menu/TopMenuOverlay.hpp>

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
            Animator(){};
            static void CreateStaticInstance();
            static Animator* GetStaticInstance() { return sInstance; };
            static inline float GetBezierEasing(float a);

            void AnimateTopMenu(Pages::TopMenuOverlay& overlay);
            void ApplyPaneColor(Pane* pane);
            void UpdateColor();
            void SetAnimationData(AnimationData* data) { currentAnimation = data; }

        private:
            static Animator* sInstance;
            AnimationData* currentAnimation;

            float frame;
            u32 currentColor;



        };
    }
}
