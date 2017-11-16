#include "ObjectsIntersection.h"


ObjectsIntersection::ObjectsIntersection(){}

ObjectsIntersection::~ObjectsIntersection()
{
	for(auto& o :m_Objects)
	{
		delete o;
		o = nullptr;
	}

	m_Objects.clear();
}

bool ObjectsIntersection::IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo)
{
	HitInfo closestHit;
	closestHit.distance = 0.f;
	for (auto& o : m_Objects)
	{
		HitInfo cHit;
		bool hit = o->IsHit(rayOrg, rayDir, cHit);
		cHit.hitObject = o;
		if (!hit || cHit.distance < 0)
			return false;

		if (cHit.distance > closestHit.distance)
			closestHit = cHit;
	}
	hitInfo = closestHit;
	return closestHit.distance > 0;
}

Vec3 ObjectsIntersection::GetNormalOnHit(Vec3 hitPosition) const
{
	return Vec3::zero;
}
