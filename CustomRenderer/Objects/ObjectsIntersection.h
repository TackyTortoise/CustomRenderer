#pragma once
#include "Object.h"
class ObjectsIntersection :
	public Object
{
public:
	ObjectsIntersection();
	~ObjectsIntersection();

	void AddObject(Object* obj) { m_Objects.push_back(obj);}

	bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo) override;
	Vec3 GetNormalOnHit(Vec3 hitPosition) const override;

private:
	std::vector<Object*> m_Objects;
};

