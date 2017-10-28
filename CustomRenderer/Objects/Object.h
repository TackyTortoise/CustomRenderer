#pragma once
#include "../Math/Math.h"
#include "../Math/Color.h"
#include "../Base/Material.h"
#include "../Base/MaterialManager.h"

class Object
{
public:
	Object() {}
	virtual ~Object(){}

	virtual bool IsHit(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& hitInfo) = 0;

	virtual const Color& GetBaseColor() const { return m_Material.color; }
	virtual void SetBaseColor(const Color& col) { m_Material.color = col; }

	Vec3 GetPosition() const { return m_Transform.GetPosition();}
	void SetPosition(const Vec3& newPos) { m_Transform.SetTranslation(newPos); }

	virtual Vec3 GetNormalOnHit(Vec3 hitPosition) const = 0;
	virtual Vec2 GetUvCoordOnHit(Vec3 hitPosition) const { return Vec2(0, 0); }

	void SetTransparent(const float v) { m_Material.transparancy = v; }
	float GetTransparancy() const { return m_Material.transparancy; }

	void SetReflective(const float v) { m_Material.reflectivity = v; }
	float GetReflective() const { return m_Material.reflectivity; }

	void SetRoughness(const float v) { m_Material.roughness = v; }
	float GetRoughness() const { return m_Material.roughness; }

	void SetRefractive(const float v) { m_Material.refractiveIndex = v; }
	float GetRefractive() const { return m_Material.refractiveIndex; }

	void SetMetallic(bool v) { m_Material.bMetallic = v; }
	Color GetSpecColor() const { return m_Material.bMetallic ? m_Material.color * (1 - m_Material.reflectivity) + Color(255) * m_Material.reflectivity : Color(255); }

	void SetShininess(float v) { m_Material.shininess = v; }
	float GetShininess() const { return m_Material.shininess; }

	void SetTexture(const char* path) { m_Material.SetTexture(path); }
	Texture* GetTexture() const { return m_Material.bUseTexture ? m_Material.texture : nullptr; }

	void SetNormalMap(const char* path) { m_Material.SetNormalMap(path); }
	Texture* GetNormalMap() const { return m_Material.bUseNormalMap ? m_Material.normalMap : nullptr; }

	void SetReflectivityMap(const char* path) { m_Material.SetReflectivityMap(path); }
	Texture* GetReflectivityMap() const { return m_Material.bUseReflectivityMap ? m_Material.reflectivityMap : nullptr; }

	void SetTransparencyMap(const char* path) { m_Material.SetTransparencyMap(path); }
	Texture* GetTransparencyMap() const { return m_Material.bUseTransparencyMap ? m_Material.transparencyMap : nullptr; }

	Material& GetMaterial() { return m_Material; }

protected:
	
	Material m_Material;

	Transform m_Transform;
};
