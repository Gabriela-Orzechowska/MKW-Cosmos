#pragma once
#include <kamek.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <SlotExpansion/CupManager.hpp>
#include <UI/CupSelect/CupSelect.hpp>
#include <main.hpp>

namespace CosmosUI{

    class CourseSelectPlus : public Pages::CourseSelect{
        public:
            CourseSelectPlus();
            UIControl * CreateControl(u32 controlId) override;
            SheetSelectControl arrows;
        private:
            //void OnActivate() override;

            void OnRightClick(u32 slotId);
            void OnLeftClick(u32 slotId);

            PtmfHolder_1A<CourseSelectPlus, void, u32> onRightClick;
            PtmfHolder_1A<CourseSelectPlus, void, u32> onLeftClick;
    };
}