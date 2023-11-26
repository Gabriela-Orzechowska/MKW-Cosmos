#ifndef _FAT_LOCK_
#define _FAT_LOCK_

#include <core/rvl/os/mutex.hpp>

namespace FAT
{   
    namespace Lock
    {
        static inline void Init(OSMutex * mutex)
        {
            OSInitMutex(mutex);
        }
        static inline void DeInit(OSMutex * mutex)
        {
            return;
        }
        static inline void Lock(OSMutex * mutex)
        {
            OSLockMutex(mutex);
        }
        static inline void Unlock(OSMutex * mutex)
        {
            OSUnlockMutex(mutex);
        }
    } // namespace Lock
    
} // namespace FAT


#endif