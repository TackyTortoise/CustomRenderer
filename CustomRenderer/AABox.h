#pragma once
#include "C:\Grad Work\CustomRenderer\CustomRenderer\Objects\Object.h"
class AABox :
	public Object
{
public:
	AABox(Vec3 pos, float width, float height, float depth, Color col = Color(255));
	~AABox();

	bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) override;
	const Vec3 GetNormalOnHit(Vec3 hitPosition) const override;
private:
	Vec3 m_Position;
	Vec3 m_BoundsMin, m_BoundsMax;
	float m_HalfWidth, m_HalfHeight, m_HalfDepth;
};

