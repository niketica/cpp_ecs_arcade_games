#include <math.h>
#include "Vec2.h"

Vec2::Vec2() {}
Vec2::Vec2(double x, double y) : x(x), y(y) {}

bool Vec2::operator == (const Vec2& vec) const
{
    return x == vec.x && y == vec.y;
}

bool Vec2::operator != (const Vec2& vec) const
{
    return x != vec.x || y != vec.y;
}

Vec2 Vec2::operator + (const Vec2& vec) const
{
    return Vec2(x + vec.x, y + vec.y);
}

Vec2 Vec2::operator - (const Vec2& vec) const
{
    return Vec2(x - vec.x, y - vec.y);
}

Vec2 Vec2::operator * (double value) const
{
    return Vec2(x * value, y * value);
}

Vec2 Vec2::operator / (double value) const
{
    return Vec2(x / value, y / value);
}

void Vec2::operator += (const Vec2& vec)
{
    x += vec.x;
    y += vec.y;
}

void Vec2::operator -= (const Vec2& vec)
{
    x -= vec.x;
    y -= vec.y;
}

void Vec2::operator *= (double value)
{
    x *= value;
    y *= value;
}

void Vec2::operator /= (double value)
{
    x /= value;
    y /= value;
}

void Vec2::normalize()
{
    x /= length();
    y /= length();
}

double Vec2::length() const
{
    return sqrtf((float)(x * x + y * y));
}

double Vec2::cross(const Vec2& vec) const
{
    return x * vec.y - y * vec.x;
}