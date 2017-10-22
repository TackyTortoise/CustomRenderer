#pragma once
#include "AABox.h"
#include "Triangle.h"

class KDNode
{
public:
	KDNode();
	~KDNode();

	AABox* m_BoundingBox = nullptr;
	KDNode* m_LeftNode = nullptr;
	KDNode* m_RightNode = nullptr;
	std::vector<Triangle*> m_Triangles;

	KDNode* BuildTree(std::vector<Triangle*>& triangles) const;

	bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo, float& shortD, float& shortBoxD);
};

