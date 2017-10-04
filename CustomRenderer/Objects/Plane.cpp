#include "Plane.h"

Plane::Plane()
{
	m_Color = Color(120, 120, 70);
}


Plane::~Plane()
{
}

bool Plane::isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	Vec3 between = m_Position - rayOrg;
	auto bdn = between.Dot(m_Normal);
	auto rdn = rayDir.Dot(m_Normal);
	if (abs(rdn) < 1e-5)
		return false;
		
	hitDistance = bdn / rdn;
	return (hitDistance >= 0);
}

