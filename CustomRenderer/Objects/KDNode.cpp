#include "KDNode.h"
#include <algorithm>


KDNode::KDNode() {}

KDNode::~KDNode()
{
	if (m_BoundingBox)
	{
		delete m_BoundingBox;
		m_BoundingBox = nullptr;
	}
}

KDNode* KDNode::BuildTree(std::vector<Triangle*>& triangles) const
{
	KDNode* node = new KDNode();
	node->m_Triangles = triangles;
	node->m_LeftNode = nullptr;
	node->m_RightNode = nullptr;
	node->m_BoundingBox = nullptr;

	if (triangles.size() == 0)
		return node;

	Vec3 midPoint = Vec3::zero;
	Vec3 minPoint = Vec3(std::numeric_limits<float>::max());
	Vec3 maxPoint = Vec3(std::numeric_limits<float>::min());

	//Get center of triangle list and bounding box
	auto ooCount = 1.f / triangles.size();
	for (auto &t : triangles)
	{
		midPoint += t->GetMidPoint() * ooCount;
		auto tMin = t->GetBoundingBox().GetMin();
		auto tMax = t->GetBoundingBox().GetMax();

		minPoint.x = std::min(minPoint.x, tMin.x);
		minPoint.y = std::min(minPoint.y, tMin.y);
		minPoint.z = std::min(minPoint.z, tMin.z);

		maxPoint.x = std::max(maxPoint.x, tMax.x);
		maxPoint.y = std::max(maxPoint.y, tMax.y);
		maxPoint.z = std::max(maxPoint.z, tMax.z);
	}

	auto center = (minPoint + maxPoint) / 2.f;
	auto width = maxPoint.x - minPoint.x;
	auto height = maxPoint.y - minPoint.y;
	auto depth = maxPoint.z - minPoint.z;
	node->m_BoundingBox = new AABox(center, width, height, depth);

	std::vector<Triangle*> leftTriangles;
	std::vector<Triangle*> rightTriangles;
	//split over longes axis
	int axis = node->m_BoundingBox->GetLongestAxis();
	for (auto &t : triangles)
	{
		switch (axis)
		{
		case 0:
			t->GetMidPoint().x <= midPoint.x ? leftTriangles.push_back(t) : rightTriangles.push_back(t);
			break;
		case 1:
			t->GetMidPoint().y <= midPoint.y ? leftTriangles.push_back(t) : rightTriangles.push_back(t);
			break;
		case 2:
			t->GetMidPoint().x <= midPoint.x ? leftTriangles.push_back(t) : rightTriangles.push_back(t);
			break;
		default:
			std::cout << "invalid longes axis of bounding box in KDNode" << std::endl;
			break;
		}
	}

	if (leftTriangles.size() > 0 && rightTriangles.size() != 0)
		node->m_LeftNode = BuildTree(leftTriangles);
	if (rightTriangles.size() > 0 && leftTriangles.size()!= 0)
		node->m_RightNode = BuildTree(rightTriangles);

	return node;
}

bool KDNode::IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo, float& shortD, float& shortBoxD)
{
	HitInfo boxHit;
	auto hitBox = m_BoundingBox->IsHit(rayOrg, rayDir, boxHit);
	if (!hitBox)
		return false;

	if (m_LeftNode != nullptr || m_RightNode != nullptr)
	{
		bool hitLeft = false, hitRight = false;
		if (m_LeftNode != nullptr)
			hitLeft = m_LeftNode->IsHit(rayOrg, rayDir, hitInfo, shortD, shortBoxD);
		if (m_RightNode != nullptr)
			hitRight = m_RightNode->IsHit(rayOrg, rayDir, hitInfo, shortD, shortBoxD);
		return hitLeft || hitRight;
	}
	//reached outward leaf
	bool hit = false;
	HitInfo hi;
	for (int i = 0; i < m_Triangles.size(); ++i)
	{
		if (m_Triangles[i]->IsHit(rayOrg, rayDir, hi))
		{
			if (hi.distance < shortD && hi.distance > 1e-5)
			{
				//save data for closest hit
				hit = true;
				shortD = hi.distance;
				hitInfo = hi;
			}
		}
	}
	return hit;

}
