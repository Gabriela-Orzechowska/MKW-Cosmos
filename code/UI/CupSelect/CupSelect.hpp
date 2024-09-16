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
#include <game/GlobalFunctions.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>
#include <game/UI/Text.hpp>
#include <game/UI/Ctrl/SheetSelect.hpp>
#include <game/Race/RaceData.hpp>
#include <main.hpp>
#include <UI/MiscUI.hpp>
#include <UI/BMG/BMG.hpp>

namespace CosmosUI
{
    enum CUP_LAYOUT{
        NORMAL = 0,
        ALPHABETICAL = 1,
    };

    #define CUPFILE_SORT_OFFSET 0x1000
    #define CUPFILE_MODE_OFFSET 0x0

    static s32 lastSelectedTrack;
    static s32 lastSelectedCup;
    static s32 lastSelectedButton;
    static s32 lastLeftCup;
    static CUP_LAYOUT currentLayout = NORMAL;

    void ExtendCourseSelectCourseInitSelf(CtrlMenuCourseSelectCourse * course);
    void ExtendCupSelectCupInitSelf(CtrlMenuCupSelectCup * cups);
    s32 AddLastLeft(s32 l);
    s32 GetLastLeft();
    void PatchCourseSelectCup();

    static inline int CorrectButtonToId(int id)
    {
        if(id < 0) id + 8;
        return (id & 1) > 0 ? (4 + (id-1)/2) : id >> 1; 
    }

    class CupSelectPlus : public Pages::CupSelect
    {
        public:
            CupSelectPlus();
            static void ChangeTPL(LayoutUIControl * ctrl, u32 id);
            UIControl * CreateControl(u32 controlId) override;

            void UpdateSelection(CtrlMenuCupSelectCup* cups, PushButton *button, u32 slotId);
            static inline CupSelectPlus* GetPage() { return MenuData::GetStaticInstance()->GetPage<CupSelectPlus>(CUP_SELECT); }
        private:
            void OnSwitchPress(u32 slotId);
            void OnActivate() override;
            void OnInit() override;
            void OnMoviesActivate(u32 param_2) override {};

            void OnRightArrowSelect(SheetSelectControl::SheetSelectButton * button, u32 slotId);
            void OnLeftArrowSelect(SheetSelectControl::SheetSelectButton * button, u32 slotId);
            void ScrollCups(s32 direction);

            PtmfHolder_1A<CupSelectPlus, void, u32> onSwitchPressHandler;
            PtmfHolder_2A<CupSelectPlus, void, SheetSelectControl::SheetSelectButton *, u32> onRightArrowSelectHandler;
            PtmfHolder_2A<CupSelectPlus, void, SheetSelectControl::SheetSelectButton *, u32> onLeftArrowSelectHandler;
            SheetSelectControl arrows;

    };

}
