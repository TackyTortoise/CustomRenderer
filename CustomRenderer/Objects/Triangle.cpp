#include "Triangle.h"
#include <iostream>

Triangle::Triangle(const Vec3& p0, const Vec3& p1, const Vec3& p2): m_P0(p0), m_P1(p1), m_P2(p2)
{
	m_Color = Color(255, 0, 255);
}

Triangle::~Triangle()
{
}

bool Triangle::isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	auto normal = GetNormalOnHit(Vec3::zero);
	float ndr = normal.Dot(rayDir);
	if (fabs(ndr) < 1e-5)
		return false;
	float d = normal.Dot(m_P0);
	float t = (normal.Dot(rayOrg) + d) / ndr;
	auto hp = rayOrg + rayDir * t;
	hitDistance = t;
	if (t < 0)
		return false;

	auto v1 = m_P1 - m_P0;
	auto v2 = m_P2 - m_P0;
	auto w = hp - m_P0;

	auto r0 = v2.Cross(normal).Normalized();
	auto d1 = v1.Dot(r0);
	auto o1 = w.Dot(r0);
	o1 /= d1;

	auto r1 = v1.Cross(normal).Normalized();
	auto d2 = v2.Dot(r1);
	auto o2 = w.Dot(r1);
	o2 /= d2;

	if (o1 > 0 && o1 < 1 && o2 > 0 && o2 < 1 && o1 + o2 < 1)
		return true;

	return false;
}

const Vec3 Triangle::GetNormalOnHit(Vec3 hitPosition) const
{
	auto v0 = m_P1 - m_P0;
	auto v1 = m_P2 - m_P0;	
	return v0.Cross(v1).Normalized();
}
