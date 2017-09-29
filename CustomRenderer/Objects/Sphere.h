#pragma once
#include "Object.h"
#include "../Math/Math.h"

class Sphere : public Object
{
public:
	Sphere();
	Sphere(const Vec3& center, const float radius, const Color color);
	~Sphere();
	bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) override;
	const Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

	void Move(Vec3& dir) { m_Position += dir; }

private:
	float m_Radius, m_Radius2;
};

