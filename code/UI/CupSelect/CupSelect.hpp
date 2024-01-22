#include <kamek.hpp>
#include <game/GlobalFunctions.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/Page/Menu/CourseSelect.hpp>
#include <game/UI/Text.hpp>
#include <game/UI/Ctrl/SheetSelect.hpp>
#include <game/Race/RaceData.hpp>
#include <main.hpp>

namespace DXUI
{
    enum CUP_LAYOUT{
        NORMAL = 0,
        ALPHABETICAL = 1,
    };

    #define CUPFILE_SORT_OFFSET 0x1000
    #define CUPFILE_MODE_OFFSET 0x0


    static CUP_LAYOUT currentLayout = NORMAL;

    class CupSelectPlus : public Pages::CupSelect
    {
        public:
            CupSelectPlus();
            //static void ChangeTPL(LayoutUIControl * ctrl, u32 id);
            UIControl * CreateControl(u32 controlId) override;
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