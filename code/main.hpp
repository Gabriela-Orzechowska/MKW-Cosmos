#ifndef _MAIN_DX_
#define _MAIN_DX_

#include <kamek.hpp>
#include <game/System/Identifiers.hpp>
#include <core/rvl/ipc/ipc.hpp>
#include <core/rvl/os/OS.hpp>

const char packFolder[20] = "/mkwdx";

s32 Open(char *path, IOS::Mode mode);

#endif //_MAIN_DX_