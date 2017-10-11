#pragma once
#include "../Math/Vec3.h"
#include "../Math/Color.h"

class Object
{
public:
	Object() {}
	virtual ~Object(){}

	virtual bool isHit(const Vec3& rayOrg, const Vec3& rayDir, float& hitDistance) = 0;

	virtual const Color& GetBaseColor() const { return m_Color; }
	virtual void SetBaseColor(const Color& col) { m_Color = col; }

	Vec3 GetPosition() const { return m_Transform.GetPosition();}
	void SetPosition(const Vec3& newPos) { m_Transform.SetTranslation(newPos); }

	virtual const Vec3 GetNormalOnHit(Vec3 hitPosition) const = 0;

	void SetTransparent(const float v) { m_Transparancy = v; }
	float GetTransparancy() const { return m_Transparancy; }

	void SetReflective(const float v) { m_Reflection = v; }
	float GetReflective() const { return m_Reflection; }

	void SetRefractive(const float v) { m_RefractionIndex = v; }
	float GetRefractive() const { return m_RefractionIndex; }

	void SetMetallic(bool v) { m_bMetallic = v; }
	Color GetSpecColor() const { return m_bMetallic ? m_Color * (1 - m_Reflection) + Color(255) * m_Reflection : Color(255); }

	void SetShininess(float v) { m_Shininess = v; }
	float GetShininess() const { return m_Shininess; }

protected:
	Color m_Color;
	float m_Transparancy = 0.f;
	float m_Reflection = 0.f;
	float m_RefractionIndex = 1.f;
	bool m_bMetallic = false;
	float m_Shininess = 50.f;
	//Vec3 m_Position;
	Transform m_Transform;
};
