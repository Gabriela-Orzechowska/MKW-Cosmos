#pragma once

#include <kamek.hpp>
#include <core/rvl/os/OS.hpp>
#include <core/egg/ProcessMeter.hpp>
#include <core/System/RKSystem.hpp>

namespace CosmosDebug
{

    class PerformanceMonitor : public EGG::ProcessMeter {
        public:
        PerformanceMonitor();
        static const s8 barCount = 3;
        static PerformanceMonitor * sInstance;
        static PerformanceMonitor * CreateInstance();
        static PerformanceMonitor * GetInstance() {return sInstance;}

        EGG::CpuMonitor objectBar;
        EGG::CpuMonitor physicsBar;
        EGG::CpuMonitor effectBar;
        bool visible;
        bool enabled;
        bool inRace;
    };

} // namespace CosmosDebug