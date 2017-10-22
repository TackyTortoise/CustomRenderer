#pragma once
#include "Object.h"
class AABox : public Object
{
public:
	AABox(Vec3 pos, float width, float height, float depth, Color col = Color(255));
	~AABox();

	bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo) override;
	Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

	Vec2 GetUvCoordOnHit(Vec3 hitPosition) const override;
	Vec3 GetMin() const { return m_BoundsMin; }
	Vec3 GetMax() const { return m_BoundsMax; }
	unsigned GetLongestAxis() const;

private:
	Vec3 m_BoundsMin, m_BoundsMax;
	float m_HalfWidth, m_HalfHeight, m_HalfDepth;
};

