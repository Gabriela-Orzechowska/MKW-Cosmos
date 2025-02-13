#pragma once
#include "System/Timer.hpp"
#include "UI/Ctrl/Manipulator.hpp"
#include "UI/Ctrl/UIControl.hpp"
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Page/Menu/Menu.hpp>


namespace Pages {
    class RankingTopTenDownloadPage : public Page {
        class RankingTopTenDownloadTimer {
        public:
            Timer timer;
            u8 unknown[0x68-0xc];
        };
    public:
        RankingTopTenDownloadPage(); //0x8060bd28
        ~RankingTopTenDownloadPage() override; //0x8060be58 vt 0x808babe0
                                               
        PageId GetNextPage() const override;
        void OnInit() override;
        void OnActivate() override;
        void AfterControlUpdate() override;
        void OnResume() override;
        int GetRuntimeTypeInfo() const override; 
        ControlsManipulatorManager inputManager;
        u32 unknown_0x54;
        u32 scope;
        CourseId course;
        u32 count;
        RankingTopTenDownloadTimer timers[10];
        u32 type;
    };


    class RankingDetailPage {
    public:

    };
}
