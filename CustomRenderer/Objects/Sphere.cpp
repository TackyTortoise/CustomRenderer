#include "Sphere.h"
#include <iostream>

Sphere::Sphere() : m_Radius(0), m_Radius2(0)
{
	m_Color = Color(0, 0, 0);
}

Sphere::Sphere(const Vec3& center, const float radius, const Color color) : m_Radius(radius), m_Radius2(radius * radius)//, m_Color(color) 
{
	SetPosition(center);
	m_Color = color;
}


Sphere::~Sphere()
{
}

bool Sphere::isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	Vec3 b = m_Transform.GetPosition() - rayOrg;
	float bdr = b.Dot(rayDir);
	if (bdr < 0) // ray pointing away from circle
		return false;
	float distanceSq = b.Length2() - bdr * bdr;
	if (distanceSq > m_Radius2)
		return false;
	float thc = sqrt(m_Radius2 - distanceSq);
	hitDistance = bdr - thc;
	return true;
}

const Vec3 Sphere::GetNormalOnHit(Vec3 hitPosition) const
{
	return (hitPosition - m_Transform.GetPosition()).Normalize();
}
