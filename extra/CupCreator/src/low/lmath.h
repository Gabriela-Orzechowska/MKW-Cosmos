#ifndef UIEDITOR_LMATH_H
#define UIEDITOR_LMATH_H

#include <cstdint>

namespace gablibela::low::math {

    struct _VEC2 {
        float x;
        float y;
    };
    struct _VEC3 {
        float x;
        float y;
        float z;
    };


    struct VEC2 : public _VEC2 {
    public:
        typedef VEC2 self_type;
        typedef float value_type;

        VEC2() : _VEC2() {x = 0.0f; y = 0.0f;}
        VEC2(const float* p) : _VEC2() { x = p[0]; y = p[1]; }
        VEC2(float fx, float fy) : _VEC2() {x = fx; y = fy;}
        VEC2(const _VEC2& v) : _VEC2() { x = v.x; y = v.y; }

    };

    struct VEC3 : public _VEC3 {
    public:
        typedef VEC3 self_type;
        typedef float value_type;

        VEC3() : _VEC3() {x = 0.0f; y = 0.0f; z = 0.0f;}
        VEC3(const float * p) : _VEC3() {x = p[0]; y = p[1]; z = p[2];}
        VEC3(float fx, float fy, float fz) : _VEC3() {x = fx; y = fy; z = fz; }
        VEC3(const _VEC3& v) : _VEC3() { x = v.x; y = v.y; z = v.z; }
    };
}



#endif //UIEDITOR_LMATH_H
