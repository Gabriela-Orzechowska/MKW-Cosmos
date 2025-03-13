#pragma once
#include "UI/MenuData/Scene.hpp"
#include <kamek.hpp>

namespace Aurora {
    enum AuroraPageId {
        START_INDEX = 0x100,
        WARNING_PAGE = START_INDEX,
        SETTINGS_MAIN,
        VARIANT_SELECT,
        SPINNER_WITH_BACKGROUND,
        LICENSE_CLASS_PROGRESS,
        LEADERBOARDS_MAIN,
        LEADERBOARDS_DETAIL,

        AURORA_PAGE_LAST,
        AURORA_PAGE_COUNT = AURORA_PAGE_LAST - START_INDEX,
    };
};
namespace Aurora {
    class ScenePlus: public Scene {
    public:
        ScenePlus() {}
        ~ScenePlus() {}

        void Deinit();

        template<class T>
        inline T* Get(PageId id) const {return (T*) this->pages[id];}
        template<class T>
        inline T* GetAuroraPage(AuroraPageId id) const {return (T*) this->auroraPages[id - START_INDEX];}
        template<class T>
        inline T* GetAuroraPage(PageId id) const {return (T*) this->auroraPages[id - START_INDEX];}

        inline void Set(Page *t, PageId id) {this->pages[id] = t;}
        inline void SetAuroraPage(Page *t, PageId id) {this->auroraPages[id - START_INDEX] = t;}

        static Page* CreatePageWithId(PageId id);

        void LoadPage(PageId id) {};
        Page* LoadAndInitPage(PageId id);

        Page* AddActivePage(PageId id);
        Page* ActivatePage(PageId id, s32 animationDirection);

    protected:
        Page* auroraPages[AURORA_PAGE_COUNT];
    };

};
