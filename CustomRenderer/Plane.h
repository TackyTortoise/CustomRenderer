#pragma once
#include "Object.h"

class Plane : public Object
{
public:
	Plane();
	~Plane();
	bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) override;
	void SetNormal(const Vec3& newNormal) { m_Normal = newNormal; }
	const Vec3 GetNormalOnHit(Vec3 hitPosition) const override { return m_Normal; }
	const Color& GetBaseColor() const override
	{
		return m_Color; 
	}
private:
	Vec3 m_Normal = Vec3(0, 1, 0);
};

