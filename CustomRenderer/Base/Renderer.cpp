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
#include <random>

#define MULTITHREADEDRENDER

Renderer* Renderer::m_Instance = nullptr;

Renderer::Renderer() {}

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
	if (m_bDone && !m_RenderSettings.autoRerender)
		return;

	std::size_t cores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> future_vector;

	for (int x = 0; x < m_RenderWidth; x += m_RegionSize.x)
	{
		for (int y = 0; y < m_RenderHeight; y += m_RegionSize.y)
		{
			//choose random pixel within block
			int rX;
			do {
				rX = x + rand() % (int)ceil(m_RegionSize.x);
			} while (rX >= m_RenderWidth);

			int rY;
			do {
				rY = y + rand() % (int)ceil(m_RegionSize.y);
			} while (rY >= m_RenderHeight);

			int pixelIndex = rX + m_RenderWidth * rY;

			if (pixelIndex > m_RenderWidth * m_RenderHeight)
				continue;

			//pixel was already drawn before
			if (m_PixelMask[pixelIndex] == 1)
			{
				//track how many misses
				++m_FalseHitCounter;
				//after as many misses as there are pixels, clear mask, start rendering again
				if (m_FalseHitCounter > m_RenderHeight * m_RenderWidth)
				{
					auto tt = Timer::GetTotalTime();
					auto rt = tt - m_LastRenderTime;
					m_LastRenderTime = tt;
					//show time it took to render frame
					std::cout << "Rerendering frame after " << rt << " seconds" << std::endl;
					m_bDone = true;
					if (m_RenderSettings.autoRerender)
						ClearPixelMask();
					return;
				}
				continue;
			}

			m_FalseHitCounter = 0;

#ifdef MULTITHREADEDRENDER
			future_vector.emplace_back(
				std::async(std::launch::async, [=]()
			{
				if (m_RenderSettings.antiAliasSampleCount <= 1)
					m_Pixels[pixelIndex] = CalculatePixelColor(rX, rY);

				else
				{
					FloatColor totalCol;
					for (int s = 0; s < m_RenderSettings.antiAliasSampleCount; ++s)
					{
						totalCol += CalculatePixelColor(rX, rY, true);
					}
					totalCol /= m_RenderSettings.antiAliasSampleCount;
					m_Pixels[pixelIndex] = totalCol.ToCharColor();
				}

				//set pixel as rendered in mask
				m_PixelMask[pixelIndex] = 1;
			}));
#else
			m_Pixels[pixelIndex] = CalculatePixelColor(rX, rY);
#endif
			}
		}
	}

void Renderer::ClearImage()
{
	ClearPixelMask(); //clear mask so pixels will be rerendered
	ClearPixelBuffer(); //set pixel buffer to black
	m_LastRenderTime = Timer::GetTotalTime(); //start rendertime from this frame
}

void Renderer::NextRenderMode()
{
	//switch buffer visualisation to next one
	int n = m_CurrentRenderMode + 1;
	n %= RenderMode::SIZE;
	m_CurrentRenderMode = static_cast<RenderMode>(n);
	ClearPixelMask();
	m_LastRenderTime = Timer::GetTotalTime();
}

void Renderer::PreviousRenderMode()
{
	//switch buffer visualisation to previous one
	int n = m_CurrentRenderMode - 1;
	if (n < 0)
		n = SIZE - 1;
	m_CurrentRenderMode = static_cast<RenderMode>(n);
	ClearPixelMask();
	m_LastRenderTime = Timer::GetTotalTime();
}

Color Renderer::CalculatePixelColor(const int x, const int y, bool multiSample)
{
	int pixelIndex = x + m_RenderWidth * y;

	//ignore if pixel has already been calculated before
	if (m_PixelMask[pixelIndex] == 1)
	{
		return m_Pixels[pixelIndex];
	}

	//Generate ray from camera to pixel
	auto cam = m_ActiveScene->GetCamera();
	Vec3 rayDir = cam->GetCameraRay(x, y, m_RenderWidth, m_RenderHeight, multiSample);

	//trace primary ray
	HitInfo traceResult;
	auto pos = cam->GetPosition();
	Object* closestObj = Trace(pos, rayDir, traceResult);

	//On object hit get color
	if (closestObj != nullptr)
	{
		//calculate pixel color
		Color col = GetHitColor(closestObj, traceResult, rayDir, 0);

		//adjust color for gamma correction if need be
		if (m_RenderSettings.enableSrgb && m_CurrentRenderMode == ALL)
		{
			col.r = pow(col.r / 255.f, 1.f / 2.2f) * 255.f;
			col.g = pow(col.g / 255.f, 1.f / 2.2f) * 255.f;
			col.b = pow(col.b / 255.f, 1.f / 2.2f) * 255.f;
		}

		//fill pixel buffer with color
		if (m_PixelMask[pixelIndex] != 1)
		{
			return col;
		}
	}
	//On miss clear pixel
	else if (m_PixelMask[pixelIndex] != 1)
	{
		return m_RenderSettings.clearColor;
	}

	++m_MaskedPixelCount;

	return m_Pixels[pixelIndex];
}

Object* Renderer::Trace(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& result, Object* ignoreObject, bool keepIgnoreDistance) const
{
	float shortD = std::numeric_limits<float>::max();
	Object* closeObject = nullptr;

	HitInfo hi;
	//check each object in scene
	for (int i = 0; i < m_ObjectCount; ++i)
	{
		//check intersection with object
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

	//calculate where object was hit in world
	if (closeObject != nullptr)
	{
		result.position = rayOrg + rayDir * shortD;
	}
	result.hitObject = closeObject;
	return closeObject;
}

Color Renderer::GetHitColor(Object* co, HitInfo& hitInfo, const Vec3& rayDir, int currentDepth)
{
	//return depth buffer if depth render mode
	if (m_CurrentRenderMode == DEPTH)
		return Color(hitInfo.distance / 100.f * 255.f);

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
		Color sampleColor = objNormal->GetPixelColor(texCoord.x, texCoord.y); //sample color from normal map
		Vec3 sampleNormal = Vec3(sampleColor.r / 255.f, sampleColor.g / 255.f, sampleColor.b / 255.f) * 2 - Vec3(1); //convert color to 0 to 1 range normal
		hitInfo.normal = Math::TangentToWorld(sampleNormal, hitInfo.normal); //convert normal from tangets space to world space
	}

	//return normals as color if normal render mode
	if (m_CurrentRenderMode == NORMALS)
	{
		auto absNormCol = hitInfo.normal * 255;
		return Color(abs(absNormCol.x), abs(absNormCol.y), abs(absNormCol.z));
	}

	//calculations influenced by light
	float diffuseIntensity = 0.f;
	float shadowFactor = 1.f;
	Color specColor;
	Color pixelColor = objectColor;
	FloatColor combinedLightColor;
	Color occludeColor;
	//calculations for each light
	for (int l = 0; l < m_LightCount; ++l)
	{
		//get light variables
		auto light = m_Lights[l];
		Color lightColor = light->GetColor();
		auto lightPos = light->GetPosition();
		Vec3 toLight = (lightPos - hitInfo.position).Normalized();

		//adjust colors to light color
		combinedLightColor += lightColor;

		//diffuse
		diffuseIntensity += Math::CalculateDiffuseIntensity(toLight, -rayDir, hitInfo.normal);

		//specular
		if (shadowFactor > .1f)
		{
			auto halfVec = (toLight + -rayDir).Normalize();
			float specStrength = Math::Clamp(hitInfo.normal.Dot(halfVec));
			specStrength = pow(specStrength, co->GetShininess());
			specColor = specColor.ClampAdd((co->GetSpecColor() * specStrength).MultiplyNormalized(lightColor));
		}

		//shadow
		HitInfo shadowHitInfo;
		auto shadowStart = hitInfo.position + toLight * 0.01f;
		Object* cs = nullptr;
		//soft shadows
		if (m_RenderSettings.shadowSampleCount > 1)
		{
			float shadowSum = 0.f;
			for (int i = 0; i < m_RenderSettings.shadowSampleCount; ++i)
			{
				auto shadowRay = (light->GetPointInAreaLight() - hitInfo.position).Normalized();
				//trace from hitpoint to light
				cs = Trace(shadowStart, shadowRay, shadowHitInfo);
				//if something was hit -> point is in shadow
				if (cs != nullptr && cs != co)
				{
					//check if hit was closer than light
					if ((shadowHitInfo.position - shadowStart).Length2() < (lightPos - hitInfo.position).Length2())
					{
						shadowSum += m_ShadowIntensity * orgNormal.Dot(shadowRay);
						//shadow color = color of lights that don't shade this point = this light color, inverted later
						occludeColor += lightColor * shadowRay.Dot(orgNormal) / m_RenderSettings.shadowSampleCount;
					}
				}
			}

			shadowSum /= m_RenderSettings.shadowSampleCount;

			//divide shadow factor by amount of light, as point is still lit by other lights in scene -> makes shadows lighter
			if (shadowSum > 1e-5f)
			{
				shadowFactor -= shadowSum / m_LightCount;
			}
		}

		//hard shadows
		else if (m_RenderSettings.shadowSampleCount == 1)
		{
			cs = Trace(shadowStart, toLight, shadowHitInfo);
			if (cs != nullptr && cs != co)
			{
				//check if not hit something behind light
				if ((shadowHitInfo.position - shadowStart).Length2() < (lightPos - hitInfo.position).Length2())
					shadowFactor -= m_ShadowIntensity / m_LightCount;
			}
		}
	}

	//Render shadows if shadow render mode
	if (m_CurrentRenderMode == SHADOWS)
		return Color(shadowFactor * 255);


	if (!(occludeColor == Color(0)) && shadowFactor < m_ShadowIntensity)
	{
		//invert light color from shadow hits
		occludeColor = (combinedLightColor - occludeColor).ToCharColor();
	}
	else
		occludeColor = (combinedLightColor).ToCharColor(); //used to multiply pixel color with light color


	//Transparancy
	float transp = co->GetTransparancy();
	if (transp > 0)
	{
		auto transTex = co->GetTransparencyMap();
		if (transTex != nullptr)
		{
			auto transSample = transTex->GetPixelColor(texCoord.x, texCoord.y);
			transp *= transSample.g / 255;
		}

		Vec3 direction = rayDir;

		//refract ray direction if necessary (refractive)
		float ior = co->GetRefractive();
		bool refractive = abs(co->GetRefractive()) - 1.f > 1e-5;
		float schlick = 0.f;
		if (refractive)
		{
			direction = Math::RefractVector(ior, direction, hitInfo.normal);
			schlick = Math::GetSchlick(rayDir, hitInfo.normal, ior);
			//if (schlick > 1.f)
			//	std::cout << "schlick pls " << schlick << std::endl;
			//if (schlick > 0.025f)
			//return Color(schlick*255);
		}
		//Trace further to get transparency color
		HitInfo transHitInfo;
		Object* transObj = Trace(hitInfo.position + rayDir * 1e-5, direction, transHitInfo);
		if (transObj != nullptr && currentDepth < m_RenderSettings.maxRenderDepth)
		{
			//adjust color to transparency hit
			diffuseIntensity += transp;
			pixelColor *= 1 - transp;
			Color transCol = GetHitColor(transObj, transHitInfo, direction, currentDepth + 1);;

			//"Fresnel" by schlick approximation reflections on edges if refractive
			if (refractive && schlick > 0.05f)
			{
				Color reflectionColor = GetReflection(rayDir, hitInfo, currentDepth);
				//return reflectionColor;
				transCol *= 1 - schlick;
				transCol += reflectionColor * schlick;
			}

			//combine pixel color with transparency color
			pixelColor = pixelColor.ClampAdd(transCol * transp);
		}
	}

	//Reflection
	float refl = co->GetReflective();
	if (refl > 1e-5 && currentDepth < m_RenderSettings.maxRenderDepth)
	{
		auto reflTex = co->GetReflectivityMap();
		if (reflTex != nullptr)
		{
			auto reflSample = reflTex->GetPixelColor(texCoord.x, texCoord.y);
			refl *= reflSample.r / 255.f;
		}

		//Get reflected color
		Color reflCol = GetReflection(rayDir, hitInfo, currentDepth);
		//adjust pixel color
		pixelColor *= 1 - refl;
		pixelColor += reflCol * refl;
	}

	//Color intensity
	diffuseIntensity = Math::Clamp(diffuseIntensity, 0.15, 1);
	pixelColor *= diffuseIntensity;
	pixelColor = pixelColor.ClampAdd(specColor);

	//adjust pixel color to light color
	Color rCol = m_LightCount > 1 ? occludeColor.MultiplyNormalized(pixelColor) * shadowFactor : (pixelColor * shadowFactor).MultiplyNormalized(combinedLightColor.ToCharColor());

	return rCol;
}

Color Renderer::GetReflection(const Vec3& rayDir, HitInfo& hitInfo, int currentDepth)
{
	//reflect incoming ray
	auto reflectedRay = Math::ReflectVector(rayDir, hitInfo.normal);
	Vec3 norm = reflectedRay, tan, bitan;
	Math::CreateCoordSystem(norm, tan, bitan);
	
	FloatColor totalCol;
	auto roughness = hitInfo.hitObject->GetRoughness();
	int rs = roughness > 0.f ? m_RenderSettings.roughnessSampleCount : 1;
	for (int i = 0; i < rs; ++i)
	{
		auto dir = Math::SampleHemisphere(norm, tan, bitan);
		dir = dir * roughness + reflectedRay * (1 - roughness);
		HitInfo reflHitInfo;
		auto startPos = hitInfo.position + dir * 1e-5;
		Object* reflectedObj = Trace(startPos, dir, reflHitInfo, hitInfo.hitObject); //ignore self when tracing reflection
		if (reflectedObj != nullptr && currentDepth < m_RenderSettings.maxRenderDepth)
		{
			//++currentDepth;
			reflHitInfo.position += dir * .1f;
			totalCol += GetHitColor(reflectedObj, reflHitInfo, dir, currentDepth + 1);
		}
	}

	//return black if over max depth or no hits
	return (totalCol / rs).ToCharColor();

	/*//reflect incoming ray
	auto reflectedRay = Math::ReflectVector(rayDir, hitInfo.normal);

	HitInfo reflHitInfo;
	auto startPos = hitInfo.position + reflectedRay * 1e-5;
	Object* reflectedObj = Trace(startPos, reflectedRay, reflHitInfo);
	if (reflectedObj != nullptr && currentDepth < m_RenderSettings.maxRenderDepth)
	{
		//++currentDepth;
		reflHitInfo.position += reflectedRay * .1f;
		return  GetHitColor(reflectedObj, reflHitInfo, reflectedRay, currentDepth + 1);
	}

	//return black if over max depth or no hits
	return Color(0);*/
}

void Renderer::ClearPixelMask()
{
	m_bDone = false;
	memset(m_PixelMask, 0, m_RenderWidth * m_RenderHeight);
	m_MaskedPixelCount = 0;
}

void Renderer::ClearPixelBuffer()
{
	m_bDone = false;
	memset(m_Pixels, 0, m_RenderWidth * m_RenderHeight * sizeof(Color));
}
