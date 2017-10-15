#pragma once
#include "Object.h"
#include "../Math/Vertex.h"

class Triangle : public Object
{
public:
	Triangle(const PosNormUVVertex& p0, const PosNormUVVertex& p1, const PosNormUVVertex& p2);
	~Triangle();

	bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) override;
	Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

	Vec2 GetUvCoordOnHit(Vec3 hitPosition) const override;
private:
	PosNormUVVertex m_P0, m_P1, m_P2;
	Vec3* m_Normal = nullptr;
	Vec3 m_LastNormal;
	Vec2 m_LastUV;
};

