#ifndef _DX_CTRLRACEBASE_
#define _DX_CTRLRACEBASE_

#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Ctrl/CtrlRace/CtrlRaceBase.hpp>
#include <game/UI/Page/RaceHUD/RaceHUD.hpp>

//Credits to Melg for SIP implementation

namespace DXUI{
    class CustomControlBuilder{
    private:
        typedef u32 (Func)();
        typedef void (Funcv)(Page * page, u32 index);

        Func * CountControls;
        Funcv * CreateControls;

        u32 controlCount;
        CustomControlBuilder * nextBuilder;
        static CustomControlBuilder * sHooks;

    public:
        static void BuildCustomRaceControl(Pages::RaceHUD * raceHudPage, u32 controlCount);
        CustomControlBuilder(Func * count, Funcv * create)
        {
            nextBuilder = sHooks;
            sHooks = this;
            CountControls = count;
            CreateControls = create;
        }
    private:
        static u32 Count()
        {
            u32 result = 0;
            for(CustomControlBuilder * builder = sHooks; builder; builder = builder->nextBuilder)
            {
                builder->controlCount = builder->CountControls();
                result += builder->controlCount;
            }
            return result;
        }
        static void CreateCustomControls(Page * page, u32 firstIdx)
        {
            u32 index = firstIdx;
            for(CustomControlBuilder * builder = sHooks; builder; builder = builder->nextBuilder)
            {
                if(builder->controlCount != 0)
                {
                    builder->CreateControls(page, index);
                    index += builder->controlCount;
                }
            }
        }
    };
}



#endif //_DX_CTRLRACEBASE_