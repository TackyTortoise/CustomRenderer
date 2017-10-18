#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include "Math.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "HitInfo.h"
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
		return Clamp(normal.Dot(lightDir), min);
	}

	static int GetSign(const int v)
	{
		return v < 0 ? -1 : 1;
	}

	static Vec3 ReflectVector(const Vec3& ray,const Vec3& axis)
	{
		return ray + axis * -2 * ray.Dot(axis);
	}

	static Vec3 RefractVector(float ior1, float ior2, Vec3 rayDir, Vec3 normal)
	{
		float ct = -rayDir.Dot(normal);
		float i1 = ior1, i2 = ior2;
		//going into or out of object
		auto refCoef = ct > 0.f ? i1 / i2 : i2 / i1;
		Vec3 refRay = rayDir * refCoef + normal * (refCoef * ct - sqrt(1 - pow(refCoef, 2) * (1 - pow(ct, 2))));
		return refRay.Normalized();
	}

	static float GetSchlick(const Vec3& incident, const Vec3& normal, float ior1, float ior2)
	{
		float r0 = (ior1 - ior2) / (ior1 + ior2);
		r0 *= r0;
		float ct = -normal.Dot(incident);
		if (ior1 > ior2)
		{
			auto refr = RefractVector(ior1, ior2, incident, normal);
			ct = -normal.Dot(refr);
		}
		return r0 + (1 - r0) * pow(1 - ct, 5);
	}

	static Vec3 TangentToWorld(const Vec3& sample, const Vec3& normal)
	{
		auto norm = normal;
		//if (norm.y - 1.f < 1)
		auto tang = abs(abs(norm.y) - 1.f) < 1e-5 ? Vec3(1,0,0) : Vec3(0, 1, 0).Cross(norm);
		auto bitan = norm.Cross(tang);
		Matrix4x4 m = Matrix4x4::Identity;

		m[0][0] = tang.x;
		m[0][1] = tang.y;
		m[0][2] = tang.z;

		m[1][0] = bitan.x;
		m[1][1] = bitan.y;
		m[1][2] = bitan.z;

		m[2][0] = norm.x;
		m[2][1] = norm.y;
		m[2][2] = norm.z;

		return m.TransformVector(sample).Normalized();
	}

#define SQRT_MAGIC_F 0x5f3759df 
	static float  sqrt2(const float x)
	{
		const float xhalf = 0.5f*x;

		union // get bits for floating value
		{
			float x;
			int i;
		} u;
		u.x = x;
		u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
		return x*u.x*(1.5f - xhalf*u.x*u.x);// Newton step, repeating increases accuracy 
	}

	static float GetRandomFloat(const float low = 0, const float high = 1)
	{
		return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	}
};