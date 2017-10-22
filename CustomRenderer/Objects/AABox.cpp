#include "AABox.h"
#include <algorithm>

AABox::AABox(Vec3 pos, float width, float height, float depth, Color col): m_HalfWidth(width / 2.f), m_HalfHeight(height / 2.f), m_HalfDepth(depth / 2.f)
{
	SetPosition(pos);
	Vec3 half = Vec3(width / 2, height / 2, depth / 2);
	m_BoundsMin = pos - half;
	m_BoundsMax = pos + half;

	m_Material.color = col;
}

AABox::~AABox()
{
}

bool AABox::IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo)
{
	//Get hit X distance
	float tMinX = (m_BoundsMin.x - rayOrg.x) / rayDir.x;
	float tMaxX = (m_BoundsMax.x - rayOrg.x) / rayDir.x;

	if (tMinX > tMaxX)
	{
		auto oldMax = tMaxX;
		tMaxX = tMinX;
		tMinX = oldMax;
	}

	//Get hit Y distance
	float tMinY = (m_BoundsMin.y - rayOrg.y) / rayDir.y;
	float tMaxY = (m_BoundsMax.y - rayOrg.y) / rayDir.y;

	if (tMinY > tMaxY)
	{
		auto oldMax = tMaxY;
		tMaxY = tMinY;
		tMinY = oldMax;
	}

	//x or y direction miss
	if (tMaxY < tMinX || tMaxX < tMinY)
		return false;

	//get where ray hit box in "2D"
	auto firstHit = tMinY > tMinX ? tMinY : tMinX;
	auto secondHit = tMaxY < tMaxX ? tMaxY : tMaxX;

	//Get hit Z distance
	float tMinZ = (m_BoundsMin.z - rayOrg.z) / rayDir.z;
	float tMaxZ = (m_BoundsMax.z - rayOrg.z) / rayDir.z;

	if (tMinZ > tMaxZ)
	{
		auto oldMax = tMaxZ;
		tMaxZ = tMinZ;
		tMinZ = oldMax;
	}

	//Z miss
	if (tMinZ > secondHit || tMaxZ < firstHit)
		return false;

	//Get final hit distances
	if (tMinZ > firstHit)
		firstHit = tMinZ;

	if (tMaxZ < secondHit)
		secondHit = tMaxZ;

	if (secondHit < firstHit)
		firstHit = secondHit;
	hitInfo.distance = firstHit;
	hitInfo.position = rayOrg + rayDir * hitInfo.distance;
	hitInfo.normal = GetNormalOnHit(hitInfo.position);
	hitInfo.uvCoordinate = GetUvCoordOnHit(hitInfo.position);

	return hitInfo.distance > 0.f;
}

Vec3 AABox::GetNormalOnHit(Vec3 hitPosition) const
{
	auto b = hitPosition - GetPosition();
	b.x = abs(abs(b.x) - m_HalfWidth) < 0.0001f ? Math::GetSign(b.x) : 0;
	b.y = abs(abs(b.y) - m_HalfHeight) < 0.0001f ? Math::GetSign(b.y) : 0;
	b.z = abs(abs(b.z) - m_HalfDepth) < 0.0001f ? Math::GetSign(b.z) : 0;
	return b;
}

Vec2 AABox::GetUvCoordOnHit(Vec3 hitPosition) const
{
	auto diff = hitPosition - m_Transform.GetPosition();
	diff.x /= m_HalfWidth;
	diff.y /= m_HalfHeight;
	diff.z /= m_HalfDepth;
	float u, v;
	//hit on left or right side
	if (abs(abs(diff.x) - 1.f) < 1e-5)
	{
		u = (diff.z + 1) / 2.f;
		v = (diff.y + 1) / 2.f;
	}
	//hit on top or bottom
	else if (abs(abs(diff.y) - 1.f) < 1e-5)
	{
		u = (diff.x + 1) / 2.f;
		v = (diff.z + 1) / 2.f;
	}
	//hit on front or back
	else
	{
		u = (diff.x + 1) / 2.f;
		v = (diff.y + 1) / 2.f;
	}

	return Vec2(u,-v);
}

unsigned AABox::GetLongestAxis() const
{
	auto max = std::max(m_HalfWidth, std::max(m_HalfHeight, m_HalfDepth));

	if (m_HalfWidth == max)
		return 0;
	if (m_HalfHeight == max)
		return 1;
	if (m_HalfDepth == max)
		return 2;

	return -1;
}
