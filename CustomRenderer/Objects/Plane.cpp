#include "Plane.h"

Plane::Plane()
{
	m_Material.color = Color(120, 120, 70);
}


Plane::~Plane()
{
}

bool Plane::IsHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	auto rdn = rayDir.Dot(m_Normal);
	//Check if perpendicular
	if (abs(rdn) < 1e-5)
		return false;

	Vec3 between = m_Transform.GetPosition() - rayOrg;
	auto bdn = between.Dot(m_Normal);
	hitDistance = bdn / rdn;
	return (hitDistance >= 0);
}

