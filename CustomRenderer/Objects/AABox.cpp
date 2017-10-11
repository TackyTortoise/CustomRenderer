#include "AABox.h"

AABox::AABox(Vec3 pos, float width, float height, float depth, Color col): m_HalfWidth(width / 2.f), m_HalfHeight(height / 2.f), m_HalfDepth(depth / 2.f)
{
	SetPosition(pos);
	Vec3 half = Vec3(width / 2, height / 2, depth / 2);
	m_BoundsMin = pos - half;
	m_BoundsMax = pos + half;

	m_Color = col;
}

AABox::~AABox()
{
}

bool AABox::isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance)
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
	hitDistance = firstHit;

	return hitDistance > 0.f;
}

const Vec3 AABox::GetNormalOnHit(Vec3 hitPosition) const
{
	auto b = hitPosition - GetPosition();
	b.x = abs(abs(b.x) - m_HalfWidth) < 0.0001f ? Math::GetSign(b.x) : 0;
	b.y = abs(abs(b.y) - m_HalfHeight) < 0.0001f ? Math::GetSign(b.y) : 0;
	b.z = abs(abs(b.z) - m_HalfDepth) < 0.0001f ? Math::GetSign(b.z) : 0;
	return b;
}
