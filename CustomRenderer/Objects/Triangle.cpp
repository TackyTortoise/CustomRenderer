#include "Triangle.h"
#include <iostream>
#include "../Base/Camera.h"

Triangle::Triangle(const PosNormUVVertex& p0, const PosNormUVVertex& p1, const PosNormUVVertex& p2): m_P0(p0), m_P1(p1), m_P2(p2)
{
	m_Material.color = Color(255, 255, 0);

	//m_Normal = new Vec3((m_P0.normal + m_P1.normal + m_P2.normal) / 3.f);
	auto v0 = m_P1.position - m_P0.position;
	auto v1 = m_P2.position - m_P0.position;
	m_Normal = new Vec3(v0.Cross(v1).Normalized());
}

Triangle::~Triangle()
{
	if (m_Normal)
	{
		delete m_Normal;
		m_Normal = nullptr;
	}
}

bool Triangle::isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
{
	auto normal = *m_Normal;
	float ndr = normal.Dot(rayDir);
	//normal pointing away from ray
	if (fabs(ndr) < 1e-5)
		return false;

	//float d = normal.Dot(m_P0.position);
	auto otp = m_P0.position - rayOrg;
	auto bdn = otp.Dot(normal);
	float t = bdn / ndr;

	//triangle behind rayorigin
	if (t < 0)
		return false;

	auto hp = rayOrg + rayDir * t;
	hitDistance = t;

	auto v1 = m_P1.position - m_P0.position;
	auto v2 = m_P2.position - m_P0.position;
	auto w = hp - m_P0.position;

	auto r0 = v2.Cross(normal);// .Normalized();
	auto d1 = v1.Dot(r0);
	auto o1 = w.Dot(r0);
	o1 /= d1;

	auto r1 = v1.Cross(normal);// .Normalized();
	auto d2 = v2.Dot(r1);
	auto o2 = w.Dot(r1);
	o2 /= d2;

	if (o1 > 0 && o1 < 1 && o2 > 0 && o2 < 1 && o1 + o2 < 1)
	{
		m_LastNormal = m_P0.normal * (1.f - o1 - o2) + m_P1.normal * o1 + m_P2.normal * o2;
		m_LastUV = m_P0.uv * (1.f - o1 - o2) + m_P1.uv * o1 + m_P2.uv * o2;
		return true;
	}

	return false;/**/
}

const Vec3 Triangle::GetNormalOnHit(Vec3 hitPosition) const
{
	return m_LastNormal;

	auto l1 = (hitPosition - m_P0.position).Length2();
	auto l2 = (hitPosition - m_P1.position).Length2();
	auto l3 = (hitPosition - m_P2.position).Length2();
	auto total = l1 + l2 + l3;
	auto t = -hitPosition.Normalized();
	auto weighted = m_P0.normal * (l1 / total) + m_P1.normal * (l2 / total) + m_P2.normal * (l3 / total);
	return weighted;// *.5f + t * .5f;
	return m_P0.normal * (l1 / total) + m_P1.normal * (l2 / total) + m_P2.normal * (l3 / total);
}

Vec2 Triangle::GetUvCoordOnHit(Vec3 hitPosition) const
{
	return m_LastUV;
}
