#pragma once

class Vec2
{
public:
    double x = 0, y = 0;

    Vec2();
    Vec2(double x, double y);

    bool operator == (const Vec2& vec) const;
    bool operator != (const Vec2& vec) const;

    Vec2 operator + (const Vec2& vec) const;
    Vec2 operator - (const Vec2& vec) const;
    Vec2 operator * (double value) const;
    Vec2 operator / (double value) const;

    void operator += (const Vec2& vec);
    void operator -= (const Vec2& vec);
    void operator *= (double value);
    void operator /= (double value);

    void normalize();

    double length() const;

    double cross(const Vec2& vec) const;
};