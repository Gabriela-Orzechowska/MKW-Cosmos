#ifndef _MAIN_DX_
#define _MAIN_DX_

#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>

namespace DX
{
    const char packFolder[20] = "/mkwdx";
    const char CommonArchive[14] = "/CommonDX.szs";
    const char UIArchive[10] = "/UIDX.szs";
    const char CourseArchive[14] = "/CourseDX.szs";

    s32 Open(char *path, IOS::Mode mode);
}


#endif //_MAIN_DX_