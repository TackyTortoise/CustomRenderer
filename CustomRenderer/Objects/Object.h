#pragma once
#include "../Math/Vec3.h"
#include "../Math/Color.h"

class Object
{
public:
	Object(){}
	virtual ~Object(){}

	virtual bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) = 0;

	virtual const Color& GetBaseColor() const { return m_Color; }

	Vec3 GetPosition() const { return m_Position; }
	void SetPosition(const Vec3& newPos) { m_Position = newPos; }

	virtual const Vec3 GetNormalOnHit(Vec3 hitPosition) const = 0;

	void SetTransparent(const float v) { m_Transparancy = v; }
	float GetTransparancy() const { return m_Transparancy; }

	void SetReflective(const float v) { m_Reflection = v; }
	float GetReflective() const { return m_Reflection; }

protected:
	Vec3 m_Position;
	Color m_Color;
	float m_Transparancy = 0.f;
	float m_Reflection = 0.f;
};
