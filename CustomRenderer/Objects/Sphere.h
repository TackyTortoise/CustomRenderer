#pragma once
#include "Object.h"
#include "../Math/Math.h"

class Sphere : public Object
{
public:
	Sphere();
	Sphere(const Vec3& center, const float radius, const Color color);
	~Sphere();
	bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo) override;
	Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

	Vec2 GetUvCoordOnHit(Vec3 hitPosition) const override;
private:
	float m_Radius, m_Radius2;
};

