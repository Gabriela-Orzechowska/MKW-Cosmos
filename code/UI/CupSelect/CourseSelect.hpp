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
