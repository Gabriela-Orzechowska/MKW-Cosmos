#pragma once
#include <kamek.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <UI/CupSelect/CupSelect.hpp>
#include <game/Sound/SoundId.hpp>
#include <main.hpp>

namespace CosmosUI{

    class CourseSelectPlus : public Pages::CourseSelect{
        public:
            CourseSelectPlus();
            UIControl* CreateControl(u32 controlId) override;
            SheetSelectControl arrows;

            static inline CourseSelectPlus* GetPage() { return MenuData::GetStaticInstance()->GetPage<CourseSelectPlus>(COURSE_SELECT); }
            void SkipAnimation() { skipNextAnim = true; }

        protected:
            //void OnActivate() override;
            void BeforeEntranceAnimations() override;

            void OnButtonClick(s32 hudSlotId, PushButton& button);
            void OnRightClick(u32 slotId) { this->SlideCup(1); }
            void OnLeftClick(u32 slotId) { this->SlideCup(-1); }
            void SlideCup(s32 offset);

            PtmfHolder_1A<CourseSelectPlus, void, u32> onRightClick;
            PtmfHolder_1A<CourseSelectPlus, void, u32> onLeftClick;
            bool skipNextAnim;
    };

    class VariantSelectPlus : public Pages::CourseSelect{
        public:
            VariantSelectPlus();
            UIControl* CreateControl(u32 controlId) override;
            SheetSelectControl arrows;
            void SetupPage(u32 slot);

            static inline VariantSelectPlus* GetPage() { return MenuData::GetStaticInstance()->GetPage<VariantSelectPlus>((PageId)Cosmos::VARIANT_SELECT); }
        protected:
            void BeforeEntranceAnimations() override;
            void OnActivate() override;
            void SetupButtons();

            void OnRightClick(u32 slotId);
            void OnLeftClick(u32 slotId);
            void OnBackPressNew(u32 hudSlotId);

            int currentSubPage;
            int subPageCount;
            int currentGroup;
            Cosmos::VariantDef* variant;
            u32 currentSlot;
            PtmfHolder_1A<VariantSelectPlus, void, u32> onRightClick;
            PtmfHolder_1A<VariantSelectPlus, void, u32> onLeftClick;
    };
}
