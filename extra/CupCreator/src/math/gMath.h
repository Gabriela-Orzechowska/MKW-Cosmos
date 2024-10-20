#ifndef GLE_MATH_H
#define GLE_MATH_H

#include "../low/lmath.h"
#include "constants.h"
#include <cmath>


namespace gablibela::math {
    class Vector2 {
    public:
        Vector2() = default;
        Vector2(float x1, float y1) {x = x1; y = y1; }
        Vector2(const low::math::VEC2& p) { x = p.x; y = p.y; }
        Vector2(const Vector2& other) {x = other.x; y = other.y; }

        bool operator==(const Vector2& other) const { return x == other.x && y == other.y;}
        bool operator!=(const Vector2& other) const { return !(x == other.x && y == other.y);}

        Vector2& operator+=(const Vector2& other) {x += other.x; y += other.y; return *this; }
        Vector2& operator-=(const Vector2& other) {x -= other.x; y -= other.y; return *this; }
        Vector2& operator*=(float f) { x *= f; y *= f; return *this; }
        Vector2& operator/=(float f) { x /= f; y /= f; return *this; }

        Vector2 operator+() const { return *this;}
        Vector2 operator-() const { return {-x, -y}; }

        Vector2 operator+ (const Vector2& other) const { return {x + other.x, y + other.y}; }
        Vector2 operator- (const Vector2& other) const { return {x - other.x, y - other.y}; }
        Vector2 operator* (float f) const { return {x * f, y * f}; }
        Vector2 operator/ (float f) const { return {x / f, y / f}; }

        float Length() const {return sqrt(x * x + y * y );}

        Vector2 Normalize() const {return *this * (1/Length()); }
        float Dot(const Vector2& other) { return x*other.x+y*other.y; }

        float x;
        float y;
    };

    class Vector3 {
    public:
        Vector3() = default;
        Vector3(float x1, float y1, float z1) { x = x1; y = y1; z = z1; }
        Vector3(const low::math::VEC3& p) { x = p.x; y = p.y; z = p.z; }
        Vector3(const Vector3& other) { x = other.x; y = other.y; z = other.z; }

        bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z;}
        bool operator!=(const Vector3& other) const { return !(x == other.x && y == other.y && z == other.z);}

        Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        Vector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
        Vector3& operator/=(float f) { x /= f; y /= f; z /= f; return *this; }

        Vector3 operator+() const { return *this; }
        Vector3 operator-() const { return {-x, -y, -z }; }

        Vector3 operator+ (const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }
        Vector3 operator- (const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }
        Vector3 operator* (float f) const { return {x * f, y * f, z * f}; }
        Vector3 operator/ (float f) const { return {x / f, y / f, z / f}; }

        float Length() const { return sqrt(x * x + y * y + z * z);}
        Vector3 Normalize() const { return *this * (1/Length()); }

        float x;
        float y;
        float z;
    };
}


#endif //GLE_MATH_H
