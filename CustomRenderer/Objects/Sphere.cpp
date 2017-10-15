#include "Sphere.h"
#include <iostream>

Sphere::Sphere() : m_Radius(0), m_Radius2(0)
{
	m_Material.color = Color(0, 0, 0);
}

Sphere::Sphere(const Vec3& center, const float radius, const Color color) : m_Radius(radius), m_Radius2(radius * radius)
{
	SetPosition(center);
	m_Material.color = color;
}


Sphere::~Sphere()
{
}

bool Sphere::IsHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	Vec3 between = m_Transform.GetPosition() - rayOrg;
	float bdr = between.Dot(rayDir);
	if (bdr < 0) //sphere behind origin
		return false;

	float centerOffsetSq = between.Length2() - bdr * bdr;
	if (centerOffsetSq > m_Radius2) //ray misses sphere
		return false;

	//calculate hit distance
	float d = Math::sqrt2(m_Radius2 - centerOffsetSq);
	hitDistance = bdr - d;
	return true;
}

const Vec3 Sphere::GetNormalOnHit(Vec3 hitPosition) const
{
	//vector from sphere center to hit position
	return (hitPosition - m_Transform.GetPosition()).Normalize();
}

Vec2 Sphere::GetUvCoordOnHit(Vec3 hitPosition) const
{
	Vec3 diff = (hitPosition - m_Transform.GetPosition()).Normalized();
	float u = 0.5f + atan2(diff.z, diff.x) / (2 * M_PI);
	float v = 0.5f - asin(diff.y) / M_PI;

	return{ u,v };
}
