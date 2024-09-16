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
        static PerformanceMonitor* sInstance;
        static PerformanceMonitor* CreateInstance();
        static PerformanceMonitor* GetInstance() {return sInstance;}

        EGG::CpuMonitor objectBar;
        EGG::CpuMonitor physicsBar;
        EGG::CpuMonitor effectBar;
        bool visible;
        bool enabled;
        bool inRace;
    };

} // namespace CosmosDebug
