#include "Renderer.h"
#include <iostream>
#include <SDL_stdinc.h>
#include "SDL.h"
#include <cmath>

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	Close();
}

void Renderer::Init(const float renderWidth, const float renderHeight, const int blockCount)
{
	Close();
	m_RenderWidth = renderWidth;
	m_RenderHeight = renderHeight;
	m_Pixels = new Color[renderWidth * renderHeight];
	m_PixelMask = new char[renderWidth * renderHeight];
	SetBlockCount(blockCount);
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
}

void Renderer::RenderScene()
{
	auto sceneCam = m_ActiveScene->GetCamera();

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
				continue;
			}

			++m_MaskedPixelCount;
			m_PixelMask[pixelIndex] = 1;

			//Generate ray from camera to pixel
			Vec3 rayDir = sceneCam->GetCameraRay(rX, rY, m_RenderWidth, m_RenderHeight);

			Vec3 orgHitPoint, orgHitNormal;
			Object* closestObj = Trace(sceneCam->GetPosition(), rayDir, orgHitPoint, orgHitNormal);

			//On object hit Color it
			if (closestObj != nullptr)
			{
				m_ReflectionDepth = m_TransparancyDepth = m_RefractionDepth = 0;
				m_Pixels[pixelIndex] = GetHitColor(closestObj, orgHitPoint, rayDir);
			}
			//On miss clear pixel
			else
				m_Pixels[pixelIndex] = m_ClearColor;
		}
	}
}

Object* Renderer::Trace(const Vec3& rayOrg, const Vec3& rayDir, Vec3& hitPoint, Vec3& hitNormal, Object* ignoreObject, bool keepIgnoreDistance) const
{
	float d = 0;
	float shortD = std::numeric_limits<float>::max();
	//float ignoreShort = shortD;
	Object* closeObject = nullptr;
	//check each object in scene
	for (int i = 0; i < m_ObjectCount; ++i)
	{
		if (m_RenderObjects[i]->isHit(rayOrg, rayDir, d) && d < shortD)
		{
			//ignore certain object e.g. self
			if (m_RenderObjects[i] != ignoreObject)
			{
				shortD = d;
				closeObject = m_RenderObjects[i];
			}
			//ignore object but still "see" it as closest
			else if (keepIgnoreDistance)
			{
				shortD = d;
			}
		}
	}

	//calculate where object was hit in world + return object normal on this point
	if (closeObject != nullptr)
	{
		hitPoint = rayOrg + rayDir * shortD;
		hitNormal = closeObject->GetNormalOnHit(hitPoint);
	}
	return closeObject;
}

Color Renderer::GetHitColor(Object* co, Vec3 hitPos, const Vec3& rayDir)
{
	auto light = m_ActiveScene->GetLights()[0];
	Color objectColor = co->GetBaseColor();
	Color ligthColor = light->GetColor();
	Color pixelColor = objectColor * (ligthColor / 255.f);
	auto lightPos = light->GetPosition();
	Vec3 toLight = (lightPos - hitPos).Normalized();
	auto hitNormal = co->GetNormalOnHit(hitPos);

	float diffuseIntensity = Math::CalculateDiffuseIntensity(toLight, -rayDir, hitNormal, .15f);

	//Transparancy
	float transp = co->GetTransparancy();
	if (transp > 0)
	{
		Vec3 direction = rayDir;
		Vec3 transHit, transNorm;
		//refract ray direction if necessary
		float ior = co->GetRefractive();
		bool refractive = abs(co->GetRefractive()) - 1.f > 1e-5;
		float schlick = 0.f;
		if (refractive)
		{
			direction = Math::RefractVector(1.f, ior, direction, hitNormal);
			schlick = Math::GetSchlick(rayDir, hitNormal, 1.f, ior);
		}
		//Trace further to get transparancy color
		Object* transObj = Trace(hitPos, direction, transHit, transNorm, co);
		if (transObj != nullptr && m_TransparancyDepth < m_MaxDepth)
		{
			++m_TransparancyDepth;
			//adjust color
			diffuseIntensity += transp;
			pixelColor *= 1 - transp;
			Color transCol = GetHitColor(transObj, transHit, direction) * (1 - schlick);
			if (refractive)
			{
				Color reflectionColor = GetReflection(rayDir, hitPos, hitNormal);
				transCol += reflectionColor * schlick;
			}
			pixelColor = pixelColor.ClampAdd(transCol * transp);
		}
	}

	//Reflection
	float refl = co->GetReflective();
	if (refl > 0.f)
	{
		pixelColor *= 1 - refl;
		Color reflCol = GetReflection(rayDir, hitPos, hitNormal);
		pixelColor += reflCol * refl;
	}	

	//shadow
	Vec3 sh, sn;
	float shadowFactor = 1.f;
	auto shadowStart = hitPos + toLight * 0.01f;
	Object* cs = nullptr;
	//soft shadows
	if (m_ShadowSamples > 1)
	{
		float shadowSum = 0.f;
		for (int i = 0; i < m_ShadowSamples; ++i)
		{
			auto shadowRay = (light->GetPointInAreaLight() - hitPos).Normalized();
			cs = Trace(shadowStart, shadowRay, sh, sn);
			if (cs != nullptr && cs != co)
			{
				if ((sh - shadowStart).Length2() < (lightPos - hitPos).Length2())
					shadowSum += m_ShadowIntensity * shadowRay.Dot(hitNormal);
			}
		}
		shadowSum /= m_ShadowSamples;
		if (shadowSum > 0.f)
		{
			shadowFactor = 1.f - shadowSum;
		}
	}
	//hard shadows
	else
	{
		cs = Trace(shadowStart, toLight, sh, sn);
		if (cs != nullptr && cs != co)
		{
			if ((sh - shadowStart).Length2() < (lightPos - hitPos).Length2())
				shadowFactor = 1 - m_ShadowIntensity;
		}
	}

	//Diffuse color
	diffuseIntensity = Math::Clamp(diffuseIntensity, 0, 1);
	pixelColor *= diffuseIntensity;

	//specular
	if (shadowFactor > .1f)
	{
		auto halfVec = (toLight + -rayDir).Normalize();
		float specStrength = Math::Clamp(hitNormal.Dot(halfVec));
		specStrength = pow(specStrength, 50);
		Color specColor = co->GetSpecColor() * specStrength * (1 - transp) * (ligthColor / 255.f);
		pixelColor = pixelColor.ClampAdd(specColor);
	}
	//pixelColor = Color(shadowFactor * 255);
	return pixelColor * shadowFactor;
}

Color Renderer::GetReflection(const Vec3& rayDir, const Vec3& hitPoint, const Vec3& hitNormal)
{
	//reflect incoming ray
	auto reflectedRay = Math::ReflectVector(rayDir, hitNormal);
	Vec3 reflHit, reflNorm;
	Object* reflectedObj = Trace(hitPoint, reflectedRay, reflHit, reflNorm);
	if (reflectedObj != nullptr && m_ReflectionDepth < m_MaxDepth)
	{
		++m_ReflectionDepth;
		return GetHitColor(reflectedObj, reflHit, reflectedRay);
	}
	return Color(0);
}

void Renderer::ClearPixelMask()
{
	for (int i = 0; i < m_RenderWidth * m_RenderHeight; ++i)
	{
		m_PixelMask[i] = 0;
	}
	m_MaskedPixelCount = 0;
}
