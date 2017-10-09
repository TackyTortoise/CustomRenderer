#pragma once
#include "Object.h"
#include "../Math/Vertex.h"

class Triangle : public Object
{
public:
	Triangle(const PosNormVertex& p0, const PosNormVertex& p1, const PosNormVertex& p2);
	~Triangle();

	bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) override;
	const Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

private:
	PosNormVertex m_P0, m_P1, m_P2;
	Vec3* m_Normal = nullptr;
	Vec3 m_LastNormal;
};

