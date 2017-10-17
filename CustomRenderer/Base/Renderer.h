#pragma once
#include "Scene.h"
#include "RenderSettings.h"
#include <atomic>
#include <thread>

class Renderer
{
public:
	Renderer();
	~Renderer();

	static Renderer* GetInstance();
	static void Destroy();
	static const RenderSettings GetRenderSettings() { return GetInstance()->m_RenderSettings; }

	void Init(const RenderSettings& rs);
	void Close();

	void SetBlockCount(const int bc);

	void SetActiveScene(const Scene* const scene);
	void RenderScene();

	Color* GetPixels() const { return m_Pixels; }

private:
	static Renderer* m_Instance;

	Object* Trace(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& result, Object* ignoreObject = nullptr, bool keepIgnoreDistance = false) const;
	
	Color GetHitColor(Object* co, HitInfo& hitInfo, const Vec3& rayDir, int& currentDepth);

	Color GetReflection(const Vec3& rayDir, HitInfo& hitInfo, int& currentDepth);

	void CalculatePixelColor(const int x, const int y);

	void ClearPixelMask();
	void ClearPixelBuffer();

	const Scene* m_ActiveScene = nullptr;
	Object*const* m_RenderObjects = nullptr;
	unsigned int m_ObjectCount = 0;

	Light* const* m_Lights = nullptr;
	unsigned int m_LightCount = 0;

	RenderSettings m_RenderSettings;

	float m_RenderWidth = 0.f, m_RenderHeight = 0.f;
	Color* m_Pixels = nullptr;

	char *m_PixelMask = nullptr;
 	unsigned m_MaskedPixelCount = 0;
	unsigned m_FalseHitCounter = 0;

	unsigned int m_BlockCount = 1;
	Vec2 m_RegionSize;

	Color m_ClearColor = Color(12, 12, 12);

	const float m_ShadowIntensity = .8f;
	char m_MaxDepth = 10;
	//short m_ReflectionDepth = 0, m_TransparancyDepth = 0, m_RefractionDepth = 0;

	int m_ShadowSamples = 1;

	bool m_bEnableSrgb = false;

	float m_LastRenderTime = 0.f;
};

