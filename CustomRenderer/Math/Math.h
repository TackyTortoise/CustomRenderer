#pragma once
#include "Vec2.h"
#include "Math.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "HitInfo.h"
#include <algorithm>
#include <SDL_stdinc.h>
#include <random>

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

	static Vec3 ReflectVector(const Vec3& ray, const Vec3& axis)
	{
		return ray + axis * -2 * ray.Dot(axis);
	}

	static Vec3 RefractVector(float ior, Vec3 rayDir, Vec3 normal)
	{
		float i1 = 1, i2 = ior;

		float ct = Clamp(rayDir.Dot(normal), -1, 1);
		Vec3 n = normal;

		//Going into object
		if (ct < 0)
			ct = -ct;
		//Going out of object
		else
		{
			std::swap(i1, i2);
			n = -normal;
		}

		float refCoef = i1 / i2;
		float k = 1 - refCoef * refCoef * (1 - ct * ct);
		return k < 0 ? Vec3::zero : refCoef * rayDir + (refCoef * ct - sqrtf(k)) * n;
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
		tangent = abs(abs(normal.y) - 1.f) < 1e-5 ?
			Vec3(1, 0, 0) :
			Vec3(0, 1, 0).Cross(normal);
		biTangent = normal.Cross(tangent);
	}

	static Vec3 TangentToWorld(const Vec3& sample, const Vec3& norm)
	{
		Vec3 tang, bitan;
		CreateCoordSystem(norm, tang, bitan);

		//Create rotation to normal space
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
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0, 1);
		auto r = (float)dis(gen);
		auto x = sqrt(r) * cos(lambda);
		auto y = sqrt(r) * sin(lambda);
		return { x, y };
	}

	static Vec3 SampleSphere()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0, 2 * M_PI);

		float t = cos((float)dis(gen));
		float lambda = (float)dis(gen);
		auto x = sqrt(1 - t*t) * cos(lambda);
		auto z = sqrt(1 - t*t) * sin(lambda);

		return{ x,t,z };
	}

	static Vec3 SampleHemisphere(const Vec3& normal = Vec3(0, 1, 0), const Vec3& tangent = Vec3(1, 0, 0), const Vec3& biTangent = Vec3(0, 0, 1))
	{
		//Sample sphere
		auto s = SampleSphere();
		//Turn around if pointing away from normal
		if (s.y < 0)
			s.y = -s.y;

		Vec3 r(s.x * tangent + s.y * normal + s.z * biTangent);

		return r;
	}

	static Vec3 SampleCone(const Vec3& normal = Vec3(0, 1, 0), const Vec3& tangent = Vec3(1, 0, 0), const Vec3& biTangent = Vec3(0, 0, 1), float angle = M_PI)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		auto ymin = cos(angle / 2.f);
		std::uniform_real_distribution<> dis(ymin, 1);

		float y = (float)dis(gen);

		Vec2 s = SampleDisk();
		auto dr = sqrt(1 - pow(y, 2.f));
		float x = s.x * dr;
		float z = s.y * dr;
		Vec3 r(x * tangent + y * normal + z * biTangent);
		return r;
	}

	static float GetRandomFloat(const float low = 0.f, const float high = 1.f)
	{
		return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	}

	static float DegToRad(const float deg)
	{
		return deg * M_PI / 180.0;
	}

	static float RadToDeg(const float rad)
	{
		return rad * (180.f / M_PI);
	}

	static float GetAngleBetweenVectorsRad(const Vec3& v1, const Vec3& v2)
	{
		auto l1 = v1.Length(), l2 = v2.Length();
		auto dp = v1.Dot(v2);
		auto cosa = dp / (l1 * l2);
		return acos(cosa);
	}

	static float GetAngleBetweenVectorsDeg(const Vec3& v1, const Vec3& v2)
	{
		auto l1 = v1.Length(), l2 = v2.Length();
		auto dp = v1.Dot(v2);
		auto cosa = dp / (l1 * l2);
		return RadToDeg(acos(cosa));
	}
};