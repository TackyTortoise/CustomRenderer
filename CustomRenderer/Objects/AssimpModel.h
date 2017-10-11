#pragma once
#include "Object.h"
#include <vector>
#include "../Math/Math.h"
#include "Triangle.h"
#include "AABox.h"

class AssimpModel :	public Object
{
public:
	AssimpModel(const char* filePath, const Vec3& pos = Vec3(0), const Vec3& rotation = Vec3(0), const Vec3& scale = Vec3(1));
	~AssimpModel();
	bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) override;
	const Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

private:
	void LoadModelFromFile(const char* filePath);
	void GenerateTriangles();

	std::vector<PosNormVertex> m_Vertices;
	std::vector<unsigned> m_Indices;
	std::vector<Triangle*> m_Triangles;
	Vec3 m_LastHitNormal;
	Vec2 m_LastHitUV;

	AABox* m_BoundingBox;
};

