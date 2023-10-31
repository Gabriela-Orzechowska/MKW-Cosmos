#pragma once

#include <core/rvl/os/mutex.hpp>

#include <modules/fat/common.hpp>

namespace FAT
{
    static inline void LockInit(OSMutex *mutex)
    {
        OSInitMutex(mutex);
    }

    static inline void LockDeinit(OSMutex *mutex)
    {
        return;
    }

    static inline void Lock(OSMutex *mutex)
    {
        OSLockMutex(mutex);
    }

    static inline void Unlock(OSMutex *mutex)
    {
        OSUnlockMutex(mutex);
    }

    }

