#ifndef _EGGDVDRIPPER_
#define _EGGDVDRIPPER_
#include "types.hpp"
#include <core/egg/Heap.hpp>
namespace EGG{
class DvdRipper{
    public:
    static void *LoadToMainRam(const char *path, void *buffer, EGG::Heap *heap, 
    u32 allocDirection, u32 offset, u32 *readBytes, u32 *size); //802226d8
};
}//namespace EGG
#endif