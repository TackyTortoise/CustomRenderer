#pragma once
#include "Object.h"
#include "../Math/Vertex.h"
#include "AABox.h"

class Triangle : public Object
{
public:
	Triangle(const PosNormUVVertex& p0, const PosNormUVVertex& p1, const PosNormUVVertex& p2);
	~Triangle();

	bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo) override;
	Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

	Vec2 GetUvCoordOnHit(Vec3 hitPosition) const override;

	Vec3 GetMidPoint();
	AABox& GetBoundingBox();

private:
	PosNormUVVertex m_P0, m_P1, m_P2;
	Vec3* m_Normal = nullptr;
	Vec3* m_MidPoint = nullptr;
	AABox* m_BoundingBox = nullptr;
	Vec3 m_LastNormal;
	Vec2 m_LastUV;
	Vec3 m_BaryRight, m_BaryUp;
	Vec3 m_Edge0, m_Edge1;
	float m_EdgeBaryRatio0, m_EdgeBaryRatio1;
};

