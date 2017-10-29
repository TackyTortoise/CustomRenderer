#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include "Math.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "HitInfo.h"
#include <algorithm>
#include <SDL_stdinc.h>
#include <random>

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

	static int GetSign(const float v)
	{
		return v < 0 ? -1 : 1;
	}

	static Vec3 ReflectVector(const Vec3& ray,const Vec3& axis)
	{
		return ray + axis * -2 * ray.Dot(axis);
	}

	static Vec3 RefractVector(float ior, Vec3 rayDir, Vec3 normal)
	{
		float cosi = Clamp(rayDir.Dot(normal), -1, 1);
		float etai = 1, etat = ior;
		Vec3 n = normal;
		if (cosi < 0) { cosi = -cosi; }
		else { std::swap(etai, etat); n = -normal; }
		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);
		return k < 0 ? 0 : eta * rayDir + (eta * cosi - sqrtf(k)) * n;

		/*float i1 = 1, i2 = ior;
		float ct = rayDir.Dot(normal);
		if (ct < 0)
		{
			std::swap(i1, i2);
		}
		else
			ct = -rayDir.Dot(normal);
		//going into or out of object
		auto refCoef = i1 / i2;
		Vec3 refRay = rayDir * refCoef + normal * (refCoef * ct - sqrt(1 - refCoef * refCoef * (1 - ct * ct)));
		return refRay.Normalized();*/
	}

	static float GetSchlick(const Vec3& incident, const Vec3& normal, float ior)
	{
		float i1 = 1, i2 = ior;
		auto idn = -incident.Dot(normal);
		if (idn < 0)
		{
			std::swap(i1, i2);
		}
		else
			idn = incident.Dot(normal);

		float r0 = pow((i1 - i2) / (i1 + i2), 2.f);
		
		return r0 + (1 - r0) * pow(1.f + idn, 5);
	}

	static void CreateCoordSystem(const Vec3& normal, Vec3& tangent, Vec3& biTangent)
	{
		tangent = abs(abs(normal.y) - 1.f) < 1e-5 ? Vec3(1, 0, 0) : Vec3(0, 1, 0).Cross(normal);
		biTangent = normal.Cross(tangent);
	}

	static Vec3 TangentToWorld(const Vec3& sample, const Vec3& norm)
	{
		//if (norm.y - 1.f < 1)
		Vec3 tang, bitan;
		CreateCoordSystem(norm, tang, bitan);

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

	static Vec2 SampleDisk()
	{
		float lambda = GetRandomFloat(0.f, 2 * M_PI);
		auto x = cos(lambda);
		auto y = sin(lambda);
		return{ x, y };
	}

	static Vec3 SampleSphere()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(-1, 1);

		float t = (float)dis(gen);
		float lambda = GetRandomFloat(0.f, 2 * M_PI);
		auto x = sqrt(1 - t*t) * cos(lambda);
		auto z = sqrt(1 - t*t) * sin(lambda);

		return{ x,t,z };
	}

	static Vec3 SampleHemisphere(const Vec3& normal = Vec3(0,1,0), const Vec3& tangent = Vec3(1,0,0), const Vec3& biTangent = Vec3(0,0,1))
	{
		auto s = SampleSphere();
		if (s.Dot(normal) < 0)
			s.y = -s.y;

		return{ s.x * tangent + s.y * normal + s.z * biTangent };
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