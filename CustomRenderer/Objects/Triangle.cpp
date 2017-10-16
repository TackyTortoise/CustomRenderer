#include "Triangle.h"
#include <iostream>
#include "../Base/Camera.h"

Triangle::Triangle(const PosNormUVVertex& p0, const PosNormUVVertex& p1, const PosNormUVVertex& p2): m_P0(p0), m_P1(p1), m_P2(p2)
{
	m_Material.color = Color(255, 255, 0);

	//calculate planar normal
	m_Edge0 = m_P1.position - m_P0.position;
	m_Edge1 = m_P2.position - m_P0.position;
	m_Normal = new Vec3(m_Edge0.Cross(m_Edge1).Normalized());

	m_BaryRight = m_Edge1.Cross(*m_Normal);
	m_BaryUp = m_Edge0.Cross(*m_Normal);

	m_EdgeBaryRatio0 = m_Edge0.Dot(m_BaryRight);
	m_EdgeBaryRatio1 = m_Edge1.Dot(m_BaryUp);
}

Triangle::~Triangle()
{
	if (m_Normal)
	{
		delete m_Normal;
		m_Normal = nullptr;
	}
}

bool Triangle::IsHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	auto normal = *m_Normal;
	float ndr = normal.Dot(rayDir);
	//normal pointing away from ray
	if (fabs(ndr) < 1e-5)
		return false;

	//check if hit plane on which triangle is situated
	auto otp = m_P0.position - rayOrg;
	auto bdn = otp.Dot(normal);
	float t = bdn / ndr;

	//triangle behind rayorigin
	if (t < 0)
		return false;

	//calculate hit position in world
	auto hp = rayOrg + rayDir * t;
	hitDistance = t;

	//check if hitpoint was inside triangle
	//auto v1 = m_P1.position - m_P0.position;
	//auto v2 = m_P2.position - m_P0.position;
	auto w = hp - m_P0.position;

	//auto  = v2.Cross(normal);// .Normalized();
	//auto d1 = m_Edge0.Dot(m_BaryRight);
	auto o1 = w.Dot(m_BaryRight);
	o1 /= m_EdgeBaryRatio0;

	//auto r1 = v1.Cross(normal);// .Normalized();
	//auto d2 = m_Edge1.Dot(m_BaryUp);
	auto o2 = w.Dot(m_BaryUp);
	o2 /= m_EdgeBaryRatio1;

	if (o1 > 0 && o1 < 1 && o2 > 0 && o2 < 1 && o1 + o2 < 1)
	{
		//save normal and uv on hit to avoid recalculations
		m_LastNormal = m_P0.normal * (1.f - o1 - o2) + m_P1.normal * o1 + m_P2.normal * o2;
		m_LastUV = m_P0.uv * (1.f - o1 - o2) + m_P1.uv * o1 + m_P2.uv * o2;
		return true;
	}

	return false;/**/
}

Vec3 Triangle::GetNormalOnHit(Vec3 hitPosition) const
{
	auto b = hitPosition - m_P0.position;
	auto o1 = b.Dot(m_BaryRight) / m_EdgeBaryRatio0;
	auto o2 = b.Dot(m_BaryUp) / m_EdgeBaryRatio1;
	return m_P0.normal * (1.f - o1 - o2) + m_P1.normal * o1 + m_P2.normal * o2;
}

Vec2 Triangle::GetUvCoordOnHit(Vec3 hitPosition) const
{
	auto b = hitPosition - m_P0.position;
	auto o1 = b.Dot(m_BaryRight) / m_EdgeBaryRatio0;
	auto o2 = b.Dot(m_BaryUp) / m_EdgeBaryRatio1;
	return m_P0.uv * (1.f - o1 - o2) + m_P1.uv * o1 + m_P2.uv * o2;;
}
