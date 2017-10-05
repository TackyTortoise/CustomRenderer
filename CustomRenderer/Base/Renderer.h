#pragma once
#include "Scene.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Init(const float renderWidth, const float renderHeight, const int blockCount = 1);
	void Close();

	void SetBlockCount(const int bc);

	void SetActiveScene(const Scene* const scene);
	void RenderScene();

	Color* GetPixels() const { return m_Pixels; }

private:
	Object* Trace(const Vec3& rayOrg, const Vec3& rayDir, Vec3& hitPoint, Vec3& hitNormal, Object* ignoreObject = nullptr, bool keepIgnoreDistance = false);
	//const Vec3 CalculateCameraRay(float x, float y, float camTan, float aspectRatio) const;
	Color GetHitColor(Object* co, Vec3 hitPos, const Vec3& rayDir);

	Color GetReflection(const Vec3& rayDir, Vec3& hitPoint, Vec3& hitNormal);

	const Scene* m_ActiveScene = nullptr;
	Object*const* m_RenderObjects = nullptr;
	unsigned int m_ObjectCount = 0;

	float m_RenderWidth = 0.f, m_RenderHeight = 0.f;
	Color* m_Pixels = nullptr;

	unsigned int m_BlockCount = 1;
	Vec2 m_RegionSize;

	Color m_ClearColor = Color(12, 12, 12);

	const float m_ShadowIntensity = .2f;
	const char m_MaxDetph = 10;
	short m_ReflectionDepth = 0, m_TransparancyDepth = 0, m_RefractionDepth = 0;
};

