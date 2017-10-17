#include "Renderer.h"
#include <iostream>
#include <SDL_stdinc.h>
#include "SDL.h"
#include <cmath>
#include "Timer.h"
#include <string>
#include <thread>
#include <atomic>
#include <future>
#include <numeric>

#define MULTITHREADEDRENDER

Renderer* Renderer::m_Instance = nullptr;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	Close();
}

Renderer* Renderer::GetInstance()
{
	if (!m_Instance)
		m_Instance = new Renderer;

	return m_Instance;
}

void Renderer::Destroy()
{
	if (!m_Instance)
		return;
	delete m_Instance;
	m_Instance = nullptr;
}

void Renderer::Init(const RenderSettings& rs)
{
	Close();

	m_RenderSettings = rs;

	m_RenderWidth = rs.texWidth;
	m_RenderHeight = rs.texHeight;
	m_Pixels = new Color[m_RenderWidth * m_RenderHeight];
	m_PixelMask = new char[m_RenderWidth * m_RenderHeight];
	SetBlockCount(rs.blockCount);

	m_MaxDepth = rs.maxRenderDepth;
	m_ShadowSamples = rs.shadowSampleCount;
	m_bEnableSrgb = rs.enableSrgb;
	m_ClearColor = rs.clearColor;

	m_LastRenderTime = Timer::GetTotalTime();
}

void Renderer::Close()
{
	if (m_Pixels != nullptr)
	{
		delete[] m_Pixels;
		m_Pixels = nullptr;
	}

	if (m_PixelMask != nullptr)
	{
		delete[] m_PixelMask;
		m_PixelMask = nullptr;
	}
}

void Renderer::SetBlockCount(const int bc)
{
	//divide rendertexture in blocks from which you choose random pixels -> faster visual result
	m_BlockCount = bc;
	m_RegionSize.x = m_RenderWidth / static_cast<float>(bc);
	m_RegionSize.y = m_RenderHeight / static_cast<float>(bc);
}

void Renderer::SetActiveScene(const Scene* const scene)
{
	if (scene->GetCamera() == nullptr)
	{
		std::cout << "New active scene has no camera" << std::endl;
		return;
	}

	m_ActiveScene = scene;
	//copy scene objects into raw pointer for efficiency
	m_RenderObjects = &scene->GetObjectPointer()[0];
	m_ObjectCount = scene->GetObjectPointer().size();

	m_Lights = &scene->GetLights()[0];
	m_LightCount = scene->GetLights().size();

	m_LastRenderTime = Timer::GetTotalTime();
	ClearPixelBuffer();
	ClearPixelMask();
}

void Renderer::RenderScene()
{
	std::size_t cores = std::thread::hardware_concurrency();
	volatile std::atomic<std::size_t> count(0);
	std::vector<std::future<void>> future_vector;
	
	for (int x = 0; x < m_RenderWidth; x += m_RegionSize.x)
	{
		for (int y = 0; y < m_RenderHeight; y += m_RegionSize.y)
		{
			//choose random pixel within block
			int rX;
			do
			{
				rX = x + rand() % (int)ceil(m_RegionSize.x);
			} while (rX >= m_RenderWidth);

			int rY;
			do
			{
				rY = y + rand() % (int)ceil(m_RegionSize.y);
			} while (rY >= m_RenderHeight);

			int pixelIndex = rX + m_RenderWidth * rY;

			if (m_PixelMask[pixelIndex] == 1)
			{
				++m_FalseHitCounter;
				if (m_FalseHitCounter >  m_RenderHeight * m_RenderWidth)
				{
					auto tt = Timer::GetTotalTime();
					auto rt = tt - m_LastRenderTime;
					m_LastRenderTime = tt;
					std::cout << "Rerendering frame after " << rt << " seconds" << std::endl;
					ClearPixelMask();
				}
				continue;
			}

			m_FalseHitCounter = 0;

#ifdef MULTITHREADEDRENDER
			if (cores--)
			{
				future_vector.emplace_back(
					std::async([=]()
				{
					CalculatePixelColor(rX, rY);
				}));
			}
#else
			CalculatePixelColor(rX, rY);
#endif
		}
	}
}

void Renderer::ClearImage()
{
	//memset(m_Pixels, 0, m_RenderWidth * m_RenderHeight * sizeof(Color));
	ClearPixelMask();
	ClearPixelBuffer();
	m_LastRenderTime = Timer::GetTotalTime();
}

void Renderer::NextRenderMode()
{
	int n = m_CurrentRenderMode + 1;
	n %= RenderMode::SIZE;
	m_CurrentRenderMode = static_cast<RenderMode>(n);
	ClearPixelMask();
	m_LastRenderTime = Timer::GetTotalTime();
}

void Renderer::PreviousRenderMode()
{
	int n = m_CurrentRenderMode - 1;
	if (n < 0)
		n = SIZE - 1;
	m_CurrentRenderMode = static_cast<RenderMode>(n);
	ClearPixelMask();
	m_LastRenderTime = Timer::GetTotalTime();
}

void Renderer::CalculatePixelColor(const int x, const int y)
{
	int pixelIndex = x + m_RenderWidth * y;

	if (m_PixelMask[pixelIndex] == 1)
	{
		return;
	}

	m_PixelMask[pixelIndex] = 1;

	//Generate ray from camera to pixel
	auto cam = m_ActiveScene->GetCamera();
	Vec3 rayDir = cam->GetCameraRay(x, y, m_RenderWidth, m_RenderHeight);
	//Vec3 orgHitPoint, orgHitNormal;
	HitInfo traceResult;
	auto pos = cam->GetPosition();
	Object* closestObj = Trace(pos, rayDir, traceResult);
	//On object hit Color it
	if (closestObj != nullptr)
	{
		//m_ReflectionDepth = m_TransparancyDepth = m_RefractionDepth = 0;
		int depth = 0;
		Color col = GetHitColor(closestObj, traceResult, rayDir, depth);
		if (m_bEnableSrgb)
		{
			col.r = pow(col.r / 255.f, 1.f / 2.2f) * 255.f;
			col.g = pow(col.g / 255.f, 1.f / 2.2f) * 255.f;
			col.b = pow(col.b / 255.f, 1.f / 2.2f) * 255.f;
		}
		m_Pixels[pixelIndex] = /*Color(abs(orgHitNormal.x) *255.f, abs(orgHitNormal.y) *255.f, abs(orgHitNormal.z) *255.f);//*/col;
	}
	//On miss clear pixel
	else
		m_Pixels[pixelIndex] = /*Color(rayDir.x *255.f, rayDir.y *255.f, rayDir.z *255.f); //*/m_ClearColor;

	++m_MaskedPixelCount;
}

Object* Renderer::Trace(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& result, Object* ignoreObject, bool keepIgnoreDistance) const
{
	//float d = 0;
	float shortD = std::numeric_limits<float>::max();
	//float ignoreShort = shortD;
	Object* closeObject = nullptr;
	//Vec3 normHit;
	Vec3 closeNorm;

	HitInfo hi;
	//check each object in scene
	for (int i = 0; i < m_ObjectCount; ++i)
	{
		if (m_RenderObjects[i]->IsHit(rayOrg, rayDir, hi) && hi.distance < shortD)
		{
			//ignore certain object e.g. self
			if (m_RenderObjects[i] != ignoreObject)
			{
				shortD = hi.distance;
				closeObject = m_RenderObjects[i];
				result = hi;
			}
			//ignore object but still consider it as closest
			else if (keepIgnoreDistance)
			{
				shortD = hi.distance;
				result = hi;
			}
		}
	}

	//calculate where object was hit in world + return object normal on this point
	if (closeObject != nullptr)
	{
		result.position = rayOrg + rayDir * shortD;
		//result.normal = closeNorm;
	}
	return closeObject;
}

Color Renderer::GetHitColor(Object* co, HitInfo& hitInfo, const Vec3& rayDir, int& currentDepth)
{
	if (m_CurrentRenderMode == DEPTH)
		return Color(hitInfo.distance / 100.f * 255.f);

	auto light = m_Lights[0];
	Color objectColor = co->GetBaseColor();

	//Sample texture if there is one
	auto objTex = co->GetTexture();
	auto texCoord = hitInfo.uvCoordinate / co->GetMaterial().GetScale();
	if (objTex != nullptr)
	{
		objectColor = objTex->GetPixelColor(texCoord.x, texCoord.y);
	}

	if (m_CurrentRenderMode == BASECOLOR)
		return objectColor;

	Vec3 orgNormal = hitInfo.normal;
	//Sample normalmap if there is one
	auto objNormal = co->GetNormalMap();
	if (objNormal != nullptr)
	{
		Color sampleColor = objNormal->GetPixelColor(texCoord.x, texCoord.y);
		Vec3 sampleNormal = Vec3(sampleColor.r / 255.f, sampleColor.g / 255.f, sampleColor.b / 255.f) * 2 - Vec3(1);
		hitInfo.normal = Math::TangentToWorld(sampleNormal, hitInfo.normal);
	}

	if (m_CurrentRenderMode == NORMALS)
	{
		auto absNormCol = hitInfo.normal * 255;
		return Color(abs(absNormCol.x), abs(absNormCol.y), abs(absNormCol.z));
	}

	Color ligthColor = light->GetColor();
	Color pixelColor = objectColor.MultiplyNormalized(ligthColor);
	auto lightPos = light->GetPosition();
	Vec3 toLight = (lightPos - hitInfo.position).Normalized();

	//shadow
	HitInfo shadowHitInfo;
	float shadowFactor = 1.f;
	auto shadowStart = hitInfo.position + toLight * 0.01f;
	Object* cs = nullptr;
	//soft shadows
	if (m_ShadowSamples > 1)
	{
		float shadowSum = 0.f;
		for (int i = 0; i < m_ShadowSamples; ++i)
		{
			auto shadowRay = (light->GetPointInAreaLight() - hitInfo.position).Normalized();
			cs = Trace(shadowStart, shadowRay, shadowHitInfo);
			if (cs != nullptr && cs != co)
			{
				if ((shadowHitInfo.position - shadowStart).Length2() < (lightPos - hitInfo.position).Length2())
					shadowSum += m_ShadowIntensity;// *shadowRay.Dot(orgNormal);
			}
		}
		shadowSum /= m_ShadowSamples;
		if (shadowSum > 0.f)
		{
			shadowFactor = 1.f - shadowSum;
		}
	}

	//hard shadows
	else if (m_ShadowSamples == 1)
	{
		cs = Trace(shadowStart, toLight, shadowHitInfo);
		if (cs != nullptr && cs != co)
		{
			if ((shadowHitInfo.position - shadowStart).Length2() < (lightPos - hitInfo.position).Length2())
				shadowFactor = 1 - m_ShadowIntensity;
		}
	}

	if (m_CurrentRenderMode == SHADOWS)
		return Color(shadowFactor * 255);

	float diffuseIntensity = Math::CalculateDiffuseIntensity(toLight, -rayDir, hitInfo.normal);

	//Transparancy
	float transp = co->GetTransparancy();
	if (transp > 0)
	{
		Vec3 direction = rayDir;
		//refract ray direction if necessary (refractive)
		float ior = co->GetRefractive();
		bool refractive = abs(co->GetRefractive()) - 1.f > 1e-5;
		float schlick = 0.f;
		if (refractive)
		{
			direction = Math::RefractVector(1.f, ior, direction, hitInfo.normal);
			schlick = Math::GetSchlick(rayDir, hitInfo.normal, 1.f, ior);
		}
		//Trace further to get transparancy color
		//Vec3 transHit, transNorm;
		HitInfo transHitInfo;
		Object* transObj = Trace(hitInfo.position, direction, transHitInfo, co);
		if (transObj != nullptr && currentDepth < m_MaxDepth)
		{
			++currentDepth;
			//adjust color
			diffuseIntensity += transp;
			pixelColor *= 1 - transp;
			Color transCol = GetHitColor(transObj, transHitInfo, direction, currentDepth) * (1 - schlick);
			//"Fresnel" reflections on edges if refractive
			if (refractive)
			{
				HitInfo reflinf;
				reflinf.position = hitInfo.position;
				reflinf.normal = transHitInfo.normal;
				Color reflectionColor = GetReflection(rayDir, reflinf, currentDepth);
				transCol += reflectionColor * schlick;
			}
			pixelColor = pixelColor.ClampAdd(transCol * transp);
		}
	}

	//Reflection
	float refl = co->GetReflective();
	if (refl > 1e-5)
	{
		pixelColor *= 1 - refl;
		HitInfo refinf;
		refinf.position = hitInfo.position - rayDir;
		refinf.normal = hitInfo.normal;
		Color reflCol = GetReflection(rayDir, refinf, currentDepth);
		pixelColor += reflCol * refl;
	}

	//Diffuse color
	diffuseIntensity = Math::Clamp(diffuseIntensity, 0.15, 1);
	pixelColor *= diffuseIntensity;

	//specular
	if (shadowFactor > .1f)
	{
		auto halfVec = (toLight + -rayDir).Normalize();
		float specStrength = Math::Clamp(hitInfo.normal.Dot(halfVec));
		specStrength = pow(specStrength, co->GetShininess());
		Color specColor = (co->GetSpecColor() * specStrength)/** (1 - transp)).MultiplyNormalized(ligthColor)*/;
		pixelColor = pixelColor.ClampAdd(specColor);
	}
	//pixelColor = Color(shadowFactor * 255);
	return  pixelColor * shadowFactor;
}

Color Renderer::GetReflection(const Vec3& rayDir, HitInfo& hitInfo, int& currentDepth)
{
	//reflect incoming ray
	auto reflectedRay = Math::ReflectVector(rayDir, hitInfo.normal);
	HitInfo reflHitInfo;
	Object* reflectedObj = Trace(hitInfo.position, reflectedRay, reflHitInfo);
	if (reflectedObj != nullptr && currentDepth < m_MaxDepth)
	{
		++currentDepth;
		reflHitInfo.position += reflectedRay * .1f;
		return GetHitColor(reflectedObj, reflHitInfo, reflectedRay, currentDepth);
	}
	return Color(0);
}

void Renderer::ClearPixelMask()
{
	/*for (int i = 0; i < m_RenderWidth * m_RenderHeight; ++i)
	{
		m_PixelMask[i] = 0;
	}*/
	memset(m_PixelMask, 0, m_RenderWidth * m_RenderHeight);
	m_MaskedPixelCount = 0;
}

void Renderer::ClearPixelBuffer()
{
	memset(m_Pixels, 0, m_RenderWidth * m_RenderHeight * sizeof(Color));
}
