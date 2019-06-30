#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <ostream>
#include "literals.h"

struct Vec2
{
    float x;
    float y;

    explicit Vec2(float _x = .0, float _y = .0) : x(_x), y(_y) { }
    inline void clear() { x = y = .0; }
    inline float length() { return sqrt(x * x + y * y); }
    inline float len2() { return x * x + y * y; }

    Vec2 operator-() const { return Vec2(-x, -y); }
    Vec2 operator+(const Vec2& b) const { return Vec2(x + b.x, y + b.y); }
    Vec2 operator-(const Vec2& b) const { return Vec2(x - b.x, y - b.y); }
    float operator*(const Vec2& b) const { return x * b.y - y * b.x; }      //叉积
    Vec2 operator*(const float b) const { return Vec2(x * b, y * b); }
    Vec2 operator/(const float b) const { return Vec2(x / b, y / b); }
    bool operator==(const Vec2& b) const { return (b - *this).len2() < EPS; }
    Vec2 rotl90() const { return Vec2(-y, x); }
    Vec2 rotr90() const { return Vec2(y, -x); }

    bool operator<(const Vec2& b) const
    {
        if (abs(x - b.x) < EPS)
            return y < b.y;
        return x < b.x;
    }
    friend std::ostream& operator<<(std::ostream& os, const Vec2& b)
    {
        os << "(" << b.x << ", " << b.y << ")";
		return os;
    }
};

#endif //VEC2_H