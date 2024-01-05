#include <kamek.hpp>
#include <game/GlobalFunctions.hpp>
#include <game/UI/Page/Menu/CupSelect.hpp>
#include <game/UI/Text.hpp>
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
        private:
            void OnSwitchPress(u32 slotId);
            void OnActivate() override;
            void OnInit() override;
            void OnMoviesActivate(u32 param_2) override {};
            PtmfHolder_1A<CupSelectPlus, void, u32> onSwitchPressHandler;
    };
}