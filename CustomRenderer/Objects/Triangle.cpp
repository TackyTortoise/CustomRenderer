#include "Triangle.h"
#include <iostream>
#include "../Base/Camera.h"
#include <algorithm>

Triangle::Triangle(const PosNormUVVertex& p0, const PosNormUVVertex& p1, const PosNormUVVertex& p2): m_P0(p0), m_P1(p1), m_P2(p2)
{
	m_Material.color = Color(255, 255, 0);

	//calculate planar normal
	m_Edge0 = m_P1.position - m_P0.position;
	m_Edge1 = m_P2.position - m_P0.position;
	m_Normal = new Vec3(m_Edge0.Cross(m_Edge1).Normalized());

	//Calculate barycentric variables
	m_BaryRight = m_Edge1.Cross(*m_Normal).Normalized();
	m_BaryUp = m_Edge0.Cross(*m_Normal).Normalized();

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
	if (m_MidPoint)
	{
		delete m_MidPoint;
		m_MidPoint = nullptr;
	}
	if (m_BoundingBox)
	{
		delete m_BoundingBox;
		m_BoundingBox = nullptr;
	}
}

bool Triangle::IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo)
{
	auto normal = *m_Normal;
	float ndr = normal.Dot(rayDir);
	//normal perpendicular to ray
	if (fabs(ndr) < 1e-5 )//|| ndr > 0)
		return false;

	//check if hit plane on which triangle is situated
	auto otp = m_P0.position - rayOrg;
	auto bdn = otp.Dot(normal);
	float t = bdn / ndr;

	//triangle plane behind rayorigin
	if (t < 0)
		return false;

	//calculate hit position in world
	auto hp = rayOrg + rayDir * t;
	hitInfo.distance = t;
	hitInfo.position = hp;

	//check if hitpoint was inside triangle
	auto w = hp - m_P0.position;

	auto o1 = w.Dot(m_BaryRight);
	o1 /= m_EdgeBaryRatio0;

	auto o2 = w.Dot(m_BaryUp);
	o2 /= m_EdgeBaryRatio1;

	if (o1 > 0 && o1 < 1 && o2 > 0 && o2 < 1 && o1 + o2 < 1)
	{
		//save normal and uv on hit to avoid recalculations
		hitInfo.normal = m_P0.normal * (1.f - o1 - o2) + m_P1.normal * o1 + m_P2.normal * o2;
		hitInfo.uvCoordinate = m_P0.uv * (1.f - o1 - o2) + m_P1.uv * o1 + m_P2.uv * o2;
		return true;
	}

	return false;
}

Vec3 Triangle::GetNormalOnHit(Vec3 hitPosition) const
{
	//get barycentric coordinates
	auto b = hitPosition - m_P0.position;
	auto o1 = b.Dot(m_BaryRight) / m_EdgeBaryRatio0;
	auto o2 = b.Dot(m_BaryUp) / m_EdgeBaryRatio1;
	//return weighted normal
	return m_P0.normal * (1.f - o1 - o2) + m_P1.normal * o1 + m_P2.normal * o2;
}

Vec2 Triangle::GetUvCoordOnHit(Vec3 hitPosition) const
{
	//get barycentric coordinates
	auto b = hitPosition - m_P0.position;
	auto o1 = b.Dot(m_BaryRight) / m_EdgeBaryRatio0;
	auto o2 = b.Dot(m_BaryUp) / m_EdgeBaryRatio1;
	//return weighted uv
	return m_P0.uv * (1.f - o1 - o2) + m_P1.uv * o1 + m_P2.uv * o2;
}

Vec3 Triangle::GetMidPoint()
{
	if (m_MidPoint)
		return *m_MidPoint;

	//Calculate midpoint if it doesn't exit yet
	m_MidPoint = new Vec3((m_P0.position + m_P1.position + m_P2.position) / 3.f);
	return *m_MidPoint;
}

AABox& Triangle::GetBoundingBox()
{
	if (m_BoundingBox)
		return *m_BoundingBox;

	//Calculate bounding box if it doesn't exist yet
	Vec3 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());

	//check point 0
	min.x = std::min(m_P0.position.x, min.x);
	min.y = std::min(m_P0.position.y, min.y);
	min.z = std::min(m_P0.position.z, min.z);

	max.x = std::max(m_P0.position.x, max.x);
	max.y = std::max(m_P0.position.y, max.y);
	max.z = std::max(m_P0.position.z, max.z);

	//check point 1
	min.x = std::min(m_P1.position.x, min.x);
	min.y = std::min(m_P1.position.y, min.y);
	min.z = std::min(m_P1.position.z, min.z);

	max.x = std::max(m_P1.position.x, max.x);
	max.y = std::max(m_P1.position.y, max.y);
	max.z = std::max(m_P1.position.z, max.z);

	//check point 2
	min.x = std::min(m_P2.position.x, min.x);
	min.y = std::min(m_P2.position.y, min.y);
	min.z = std::min(m_P2.position.z, min.z);

	max.x = std::max(m_P2.position.x, max.x);
	max.y = std::max(m_P2.position.y, max.y);
	max.z = std::max(m_P2.position.z, max.z);

	//create box
	auto center = (min + max) / 2.f;
	auto width = max.x - min.x;
	auto height = max.y - min.y;
	auto depth = max.z - min.z;
	m_BoundingBox = new AABox(center, width, height, depth);
	return *m_BoundingBox;
}
