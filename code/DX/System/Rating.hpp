#include <kamek.hpp>


extern "C"{
    int DWC_AddMatchKeyInt(u32 id, char * param_2, u32 * valuePointer);
    float CalcRating(float x);
}