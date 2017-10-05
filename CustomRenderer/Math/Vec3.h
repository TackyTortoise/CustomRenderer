#pragma once
#include <ostream>

struct Vec3
{
public:
	float x, y, z;

	Vec3() : x(0), y(0), z(0) {}
	Vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
	Vec3(float v) :x(v), y(v), z(v) {}

#pragma region operators
	Vec3 operator* (const float& v) const {return Vec3(x * v, y * v, z * v);}

	Vec3 operator* (const Vec3& o) const {return Vec3(x * o.x, y * o.y, z * o.x);}

	Vec3 operator/ (const Vec3& o) const {return Vec3(x / o.x, y / o.y, z / o.x);}

	float Dot(const Vec3& o) const {return x * o.x + y * o.y + z * o.z;}

	Vec3 operator+(const Vec3& o) const {return Vec3(x + o.x, y + o.y, z + o.z);}

	Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z);}

	Vec3 operator+= (const Vec3& o)	
	{
		x += o.x, y += o.y,	z += o.z;
		return *this;
	}

	Vec3 operator-= (const Vec3& o)
	{
		x -= o.x, y -= o.y,	z -= o.z;
		return *this;
	}

	Vec3 operator*= (const Vec3& o)
	{
		x *= o.x, y *= o.y, z *= o.z;
		return *this;
	}

	Vec3 operator/= (const Vec3& o)
	{
		x /= o.x, y /= o.y, z /= o.z;
		return *this;
	}

	Vec3 operator-() const { return Vec3(-x, -y, -z); }
#pragma endregion

	float Length2() const { return x*x + y*y + z*z; }
	float Length() const { return sqrt(Length2()); }

	Vec3& Normalize()
	{
		auto l = Length();
		x /= l;
		y /= l;
		z /= l;
		return *this;
	}

	Vec3 Normalized() const
	{
		Vec3 r = *this;
		r.Normalize();
		return r;
	}

	friend std::ostream & operator << (std::ostream &str, const Vec3 &v)
	{
		str << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
		return str;
	}
};
