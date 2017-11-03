#include "Plane.h"

Plane::Plane()
{
	m_Material.color = Color(120, 120, 70);
	m_Right = Vec3(m_Normal.y, -m_Normal.x, m_Normal.z);
	m_Up = m_Right.Cross(m_Normal).Normalized();
}

Plane::~Plane(){}

bool Plane::IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo)
{
	auto rdn = rayDir.Dot(m_Normal);
	//Check if backside or perpendicular hit
	if (rdn > -1e-5)
		return false;

	Vec3 between = m_Transform.GetPosition() - rayOrg;
	auto bdn = between.Dot(m_Normal);

	//Save hit info
	hitInfo.distance = bdn / rdn;
	hitInfo.normal = m_Normal;
	hitInfo.position = rayOrg + rayDir * hitInfo.distance;
	auto ctp = hitInfo.position - m_Transform.GetPosition();
	hitInfo.uvCoordinate = Vec2(ctp.Dot(m_Right), ctp.Dot(m_Up));
	return hitInfo.distance >= 0;
}

