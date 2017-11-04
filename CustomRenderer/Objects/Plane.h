#pragma once
#include "Object.h"

class Plane : public Object
{
public:
	Plane();
	~Plane();
	bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo) override;
	void SetNormal(const Vec3& newNormal);
	Vec3 GetNormalOnHit(Vec3 hitPosition) const override { return m_Normal; }
private:
	Vec3 m_Normal = Vec3(0, 1, 0);
	Vec3 m_Right, m_Up;
};

