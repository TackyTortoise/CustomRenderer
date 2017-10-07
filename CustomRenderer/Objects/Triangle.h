#pragma once
#include "Object.h"

class Triangle : public Object
{
public:
	Triangle(const Vec3& p0, const Vec3& p1, const Vec3& p2);
	~Triangle();

	bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) override;
	const Vec3 GetNormalOnHit(Vec3 hitPosition) const override;
private:
	Vec3 m_P0, m_P1, m_P2;
};

