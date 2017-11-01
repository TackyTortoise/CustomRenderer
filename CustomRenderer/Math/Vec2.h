#pragma once
#include <ostream>

class Vec2
{
public:
	float x, y;

	Vec2() : x(0), y(0) {}
	Vec2(float xx, float yy) : x(xx), y(yy){}
	Vec2(float v) :x(v), y(v) {}

#pragma region operators
	Vec2 operator* (const float& v) const { return Vec2(x * v, y * v); }

	Vec2 operator* (const Vec2& o) const { return Vec2(x * o.x, y * o.y); }

	Vec2 operator/ (const float& v) const { return Vec2(x / v, y / v); }

	Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }

	Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }

	Vec2 operator+= (const Vec2& o)
	{
		x += o.x;
		y += o.y;
		return *this;
	}

	Vec2 operator-= (const Vec2& o)
	{
		x -= o.x;
		y -= o.y;
		return *this;
	}

	Vec2 operator*= (const Vec2& o)
	{
		x *= o.x;
		y *= o.y;
		return *this;
	}

	Vec2 operator/= (const Vec2& o)
	{
		x /= o.x;
		y /= o.y;
		return *this;
	}

	Vec2 operator-() const { return Vec2(-x, -y); }
#pragma endregion

	friend std::ostream & operator << (std::ostream &str, const Vec2 &v)
	{
		str << "[ " << v.x << ", " << v.y << " ]";
		return str;
	}
};
