#pragma once

#include <kamek.hpp>
#include <core/rvl/os/OS.hpp>
#include <core/egg/ProcessMeter.hpp>
#include <core/System/RKSystem.hpp>

// Thanks to Melg for allowing me to check CTTP's PerformanceMonitor, wasnt aware what i was doing wrong

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