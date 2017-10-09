#pragma once
#include "Object.h"
#include <vector>
#include "../Math/Vertex.h"
#include "Triangle.h"

class AssimpModel :	public Object
{
public:
	AssimpModel(const char* filePath, Vec3 pos);
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
};

