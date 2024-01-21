#include <kamek.hpp>
#include <core/System/SystemManager.hpp>

#define TEST_REGION 866

void ChangeGameRegion(u32 regionID)
{
    extern u32 pr_patch1_r5;
    extern u32 pr_patch2_r6;
    extern u32 pr_patch3_r7;
    extern u32 pr_patch4_r7;
    extern u32 pr_patch5_r7;
    extern u32 pr_patch6_r7;
    extern u32 pr_patch7_r4;
    extern u32 pr_patch8_r4;

    pr_patch1_r5 = 0x38A00000 | regionID;
    pr_patch2_r6 = 0x38C00000 | regionID;
    pr_patch3_r7 = 0x38E00000 | regionID;
    pr_patch4_r7 = 0x38E00000 | regionID;
    pr_patch5_r7 = 0x38E00000 | regionID;
    pr_patch6_r7 = 0x38E00000 | regionID;
    pr_patch7_r4 = 0x38800000 | regionID;
    pr_patch8_r4 = 0x38800000 | regionID;
}

void ChangeRegionAtBoot(){
    ChangeGameRegion(TEST_REGION);
}

kmWrite16(0x8088fe30, 0x6478); //rksys -> dxsys

static BootHook bhChangeRegionAtBoot(ChangeRegionAtBoot, LOW);