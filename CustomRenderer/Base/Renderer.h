#pragma once
#include "Scene.h"
#include "RenderSettings.h"
#include <atomic>
#include <thread>

enum RenderMode
{
	ALL,
	BASECOLOR,
	NORMALS,
	SHADOWS,
	DEPTH,
	UV,
	SIZE,
};

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

	void ClearImage();

	void NextRenderMode();
	void PreviousRenderMode();

	void DrawText(SDL_Renderer* renderer, const char* aText, const char* aFontFile, int aX, int aY);

private:
	static Renderer* m_Instance;

	void RenderPixel(const int x, const int y);

	Object* Trace(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& result, const Object* ignoreObject = nullptr, bool keepIgnoreDistance = false) const;
	
	Color GetHitColor(Object* co, HitInfo& hitInfo, const Vec3& rayDir, int currentDepth);

	void CalculateDirectLighting(const Vec3& rayDir, const HitInfo& hitInfo, const Vec3& orgNormal, float& diffuseIntensity, float& shadowFactor, Color& specColor, FloatColor& combinedLightColor, Color& occludeColor) const;
	void CalculateTransparency(Color& pixelColor, float transp, const Vec3& rayDir, const HitInfo& hitInfo, float& diffuseIntensity, const int currentDepth);
	void CalculateReflection(Color& pixelColor, float refl, const Vec3& rayDir, const HitInfo& hitInfo, const int currentDepth);

	Color GetObjectColor(const HitInfo& hitInfo) const;
	Vec3 GetHitNormal(const HitInfo& hitInfo) const;

	Color GetReflection(const Vec3& rayDir, const HitInfo& hitInfo, int currentDepth);

	Color CalculatePixelColor(const int x, const int y);

	FloatColor GetGlobalIllumination(const HitInfo& hitInfo, const unsigned depth);

	int CheckUndrawn();
	
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
	unsigned m_FalseHitCounter = 0;

	unsigned int m_BlockCount = 1;
	Vec2 m_RegionSize;

	const float m_ShadowIntensity = .8f;

	float m_LastRenderTime = 0.f;

	RenderMode m_CurrentRenderMode = ALL;

	bool m_bDone = false;
	bool m_FullRender = false;
};

