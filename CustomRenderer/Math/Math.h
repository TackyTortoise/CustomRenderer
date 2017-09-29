#pragma once
#include "Vec3.h"
#include "Vec2.h"
//#include "Color.h"

class Math
{
public:
	static float Clamp(const float f, const float min = 0.f, const float max = 1.f)
	{
		if (f < min) 
			return min;
		if (f > max)
			return max;
		return f;
	}

	static float CalculateDiffuseIntensity(const Vec3& lightDir, const Vec3& viewDir, const Vec3& normal, float min = 0)
	{
		auto halfVec = (lightDir + viewDir).Normalize();
		return Clamp(normal.Dot(halfVec), min);
	}
};