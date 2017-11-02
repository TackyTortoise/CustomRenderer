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
#include <SDL_ttf.h>

#define MULTITHREADEDRENDER

Renderer* Renderer::m_Instance = nullptr;

Renderer::Renderer(){}

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
	{
		//Stop rendering if not on auto rerender
		if (m_FullRender)
			return;

		//Check for missed pixels during last render
		int missed = CheckUndrawn();
		m_FullRender = true;
		if (missed > 0)
		{
			std::cout << "Missed " << missed << " pixels while rendering, rendering them now" << std::endl;
			m_FullRender = false;
		}
		return;
	}

	std::vector<std::future<void>> future_vector;

	for (int x = 0; x < m_RenderWidth; x += m_RegionSize.x)
	{
		for (int y = 0; y < m_RenderHeight; y += m_RegionSize.y)
		{
			//Choose random x within block
			int rX = x + rand() % (int)ceil(m_RegionSize.x);
			rX %= (int)m_RenderWidth; //Just for safety

			//Choose random y within block
			int rY = y + rand() % (int)ceil(m_RegionSize.y);
			rY %= (int)m_RenderHeight; //Just for safety

			int pixelIndex = rX + m_RenderWidth * rY;

			if (pixelIndex > m_RenderWidth * m_RenderHeight) //Another safety check
				continue;

			//Pixel was already drawn before
			if (m_PixelMask[pixelIndex] == 1)
			{
				//Track how many misses
				++m_FalseHitCounter;
				//After as many misses as there are pixels
				if (m_FalseHitCounter > m_RenderHeight * m_RenderWidth)
				{
					auto tt = Timer::GetTotalTime();
					auto rt = tt - m_LastRenderTime;
					m_LastRenderTime = tt;
					//Report time it took to render frame
					std::cout << (m_RenderSettings.autoRerender ? "Rerendering" : "Rendered") << " frame after " << rt << " seconds" << std::endl;
					m_bDone = true;
					//Clear mask to start rendering again if autorender
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
				RenderPixel(rX, rY);
			}));
#else
			RenderPixel(rX, rY);
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

Color Renderer::CalculatePixelColor(const int x, const int y)
{
	int pixelIndex = x + m_RenderWidth * y;

	//Ignore if pixel has already been calculated before
	if (m_PixelMask[pixelIndex] == 1)
	{
		return m_Pixels[pixelIndex];
	}

	//Generate ray from camera to pixel
	auto cam = m_ActiveScene->GetCamera();
	auto aa = m_RenderSettings.antiAliasSampleCount > 0 && m_CurrentRenderMode == ALL;
	Vec3 rayDir =
		m_RenderSettings.dofSampleCount > 0 && m_CurrentRenderMode == ALL ?
		cam->GetCameraRayDOF(x, y, m_RenderWidth, m_RenderHeight, aa) :
		cam->GetCameraRay(x, y, m_RenderWidth, m_RenderHeight, aa);


	//Trace primary ray
	HitInfo traceResult;
	auto pos = cam->GetPosition();
	Object* closestObj = Trace(pos, rayDir, traceResult);

	//On object hit get color
	if (closestObj != nullptr)
	{
		//Calculate pixel color
		Color col = GetHitColor(closestObj, traceResult, rayDir, 0);

		//Adjust color for gamma correction if need be
		if (m_RenderSettings.enableSrgb && m_CurrentRenderMode == ALL)
		{
			col.r = pow(col.r / 255.f, 1.f / 2.2f) * 255.f;
			col.g = pow(col.g / 255.f, 1.f / 2.2f) * 255.f;
			col.b = pow(col.b / 255.f, 1.f / 2.2f) * 255.f;
		}

		//Fill pixel buffer with color
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

	return m_Pixels[pixelIndex];
}

void Renderer::RenderPixel(const int x, const int y)
{
	int pixelIndex = x + m_RenderWidth * y;

	//Ignore AA and DoF in buffer renders
	if (m_CurrentRenderMode != ALL)
	{
		m_Pixels[pixelIndex] = CalculatePixelColor(x, y);
		m_PixelMask[pixelIndex] = 1;
		return;
	}

	//Total render
	FloatColor totalCol;

	//Iterations for Depth of Field
	auto dofMulti = m_RenderSettings.dofSampleCount > 0 && m_ActiveScene->GetCamera()->IsDOFEnabled() ?
		m_RenderSettings.dofSampleCount : 1;

	//Iterations for Anti-Aliasing
	auto aaMulti = m_RenderSettings.antiAliasSampleCount > 0 ? m_RenderSettings.antiAliasSampleCount : 1;

	//Total iterations
	unsigned iterations = aaMulti * dofMulti;

	//Calculate pixel color for all iterations
	for (int s = 0; s < iterations; ++s)
	{
		totalCol += CalculatePixelColor(x, y);
	}
	//Average color
	totalCol /= iterations;

	//Update pixel buffer
	m_Pixels[pixelIndex] = totalCol.ToCharColor();

	//Set pixel as rendered in mask
	m_PixelMask[pixelIndex] = 1;
}

Object* Renderer::Trace(const Vec3& rayOrg, const Vec3& rayDir, HitInfo& result, const Object* ignoreObject, bool keepIgnoreDistance) const
{
	float shortD = std::numeric_limits<float>::max();
	Object* closeObject = nullptr;

	HitInfo hi;
	//Check each object in scene
	for (int i = 0; i < m_ObjectCount; ++i)
	{
		//Check intersection with object
		if (m_RenderObjects[i]->IsHit(rayOrg, rayDir, hi) && hi.distance < shortD)
		{
			//Ignore certain object e.g. self when reflecting
			if (m_RenderObjects[i] != ignoreObject)
			{
				shortD = hi.distance;
				closeObject = m_RenderObjects[i];
				result = hi;
			}
			//Ignore object but still consider it as closest
			else if (keepIgnoreDistance)
			{
				shortD = hi.distance;
				result = hi;
			}
		}
	}

	//Calculate where object was hit in world
	if (closeObject != nullptr)
	{
		result.position = rayOrg + rayDir * shortD;
	}
	result.hitObject = closeObject;
	return closeObject;
}

Color Renderer::GetHitColor(Object* co, HitInfo& hitInfo, const Vec3& rayDir, int currentDepth)
{
	//Return depth buffer if depth render mode
	if (m_CurrentRenderMode == DEPTH)
		return Color(hitInfo.distance / 100.f * 255.f);

	//Return UV's if UV render mode
	if (m_CurrentRenderMode == UV)
		return Color(hitInfo.uvCoordinate.x * 255, hitInfo.uvCoordinate.y * 255, 0);

	Color objectColor = GetObjectColor(hitInfo);

	if (m_CurrentRenderMode == BASECOLOR)
		return objectColor;

	Color pixelColor = objectColor;

	//Sample normalmap if there is one
	Vec3 orgNormal = hitInfo.normal;
	hitInfo.normal = GetHitNormal(hitInfo);

	//Return normals as color if normal render mode
	if (m_CurrentRenderMode == NORMALS)
	{
		auto absNormCol = hitInfo.normal;
		return Color(abs(absNormCol.x * 255), abs(absNormCol.y * 255), abs(absNormCol.z * 255));
	}

	//Variables for calculations influenced by light
	float diffuseIntensity = 0.f;
	float shadowFactor = 1.f;
	Color specColor;
	FloatColor combinedLightColor;
	Color occludeColor;

	//Calculations for each light
	CalculateDirectLighting(rayDir, hitInfo, orgNormal, diffuseIntensity, shadowFactor, specColor, combinedLightColor, occludeColor);

	//Render shadows if shadow render mode
	if (m_CurrentRenderMode == SHADOWS)
		return Color(shadowFactor * 255);

	//Transparency
	float transp = co->GetTransparency();
	if (transp > 0)
	{
		CalculateTransparency(pixelColor, transp, rayDir, hitInfo, diffuseIntensity, currentDepth);
	}

	//Reflection
	float refl = co->GetReflective();
	if (refl > 0 && currentDepth < m_RenderSettings.maxRenderDepth)
	{
		CalculateReflection(pixelColor, refl, rayDir, hitInfo, currentDepth);
	}

	//Color intensity
	diffuseIntensity = Math::Clamp(diffuseIntensity, 0.15, 1);

	//adjust pixel color to light color
	FloatColor lightColor = m_LightCount > 1 ? occludeColor : combinedLightColor;
	FloatColor directLightColor = (lightColor * diffuseIntensity * shadowFactor).ToCharColor();
	FloatColor indirectLightColor = currentDepth < m_RenderSettings.GIMaxDepth ? GetGlobalIllumination(hitInfo, 0) : Color(0);
	FloatColor totalLightColor = directLightColor + indirectLightColor;

	FloatColor pc = pixelColor.MultiplyNormalized(totalLightColor.ToCharColor());
	Color rCol = pc.ToCharColor().ClampAdd(specColor);

	return rCol;
}

void Renderer::CalculateDirectLighting(const Vec3& rayDir, const HitInfo& hitInfo, const Vec3& orgNormal, float& diffuseIntensity, float& shadowFactor, Color& specColor, FloatColor& combinedLightColor, Color& occludeColor) const
{
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
			specStrength = pow(specStrength, hitInfo.hitObject->GetShininess());
			specColor = specColor.ClampAdd((hitInfo.hitObject->GetSpecColor() * specStrength).MultiplyNormalized(lightColor));
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
				if (cs != nullptr && cs != hitInfo.hitObject)
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
			if (cs != nullptr && cs != hitInfo.hitObject)
			{
				//check if not hit something behind light
				if ((shadowHitInfo.position - shadowStart).Length2() < (lightPos - hitInfo.position).Length2())
					shadowFactor -= m_ShadowIntensity / m_LightCount;
			}
		}
	}

	if (!(occludeColor == Color(0)) && shadowFactor < m_ShadowIntensity)
	{
		//invert light color from shadow hits
		occludeColor = (combinedLightColor - occludeColor).ToCharColor();
	}
	else
		occludeColor = combinedLightColor.ToCharColor(); //used to multiply pixel color with light color
}

void Renderer::CalculateTransparency(Color& pixelColor, float transp, const Vec3& rayDir, const HitInfo& hitInfo, float& diffuseIntensity, const int currentDepth)
{
	auto transTex = hitInfo.hitObject->GetTransparencyMap();
	//Sample transparency texture if there is one
	if (transTex != nullptr)
	{
		Vec2 texCoord = hitInfo.uvCoordinate / hitInfo.hitObject->GetMaterial().GetScale();
		auto transSample = transTex->GetPixelColor(texCoord.x, texCoord.y);
		transp *= transSample.g / 255;
	}

	Vec3 direction = rayDir;

	//Refract ray direction if necessary (refractive)
	float ior = hitInfo.hitObject->GetRefractive();
	bool refractive = abs(hitInfo.hitObject->GetRefractive()) - 1.f > 1e-5;
	float schlick = 0.f;
	if (refractive)
	{
		direction = Math::RefractVector(ior, direction, hitInfo.normal);
		schlick = Math::GetSchlick(rayDir, hitInfo.normal, ior);
	}

	//Trace further to get transparency color
	HitInfo transHitInfo;
	Object* transObj = Trace(hitInfo.position + rayDir * 1e-5, direction, transHitInfo);
	if (transObj != nullptr && currentDepth < m_RenderSettings.maxRenderDepth)
	{
		//Adjust diffuse to transparency hit
		diffuseIntensity += transp;
		FloatColor transCol = GetHitColor(transObj, transHitInfo, direction, currentDepth + 1);;

		//"Fresnel" by schlick approximation reflections on edges if refractive
		if (refractive && schlick > 0.05f)
		{
			Color reflectionColor = GetReflection(rayDir, hitInfo, currentDepth);
			transCol *= 1 - schlick;
			transCol += reflectionColor * schlick;
		}

		//Combine pixel color with transparency color
		pixelColor *= 1 - transp;
		pixelColor = pixelColor.ClampAdd((transCol * transp).ToCharColor());
	}
}

void Renderer::CalculateReflection(Color& pixelColor, float refl, const Vec3& rayDir, const HitInfo& hitInfo, const int currentDepth)
{
	auto reflTex = hitInfo.hitObject->GetReflectivityMap();
	//Sample reflectivity map if there is one
	if (reflTex != nullptr)
	{
		Vec2 texCoord = hitInfo.uvCoordinate / hitInfo.hitObject->GetMaterial().GetScale();
		auto reflSample = reflTex->GetPixelColor(texCoord.x, texCoord.y);
		refl *= reflSample.r / 255.f;
	}

	//Get schlick for edge reflections
	auto schlick = Math::GetSchlick(rayDir, hitInfo.normal, 1.f);
	refl = std::max(refl, schlick);

	//Get reflected color
	Color reflCol = GetReflection(rayDir, hitInfo, currentDepth);

	//Adjust pixel color
	pixelColor *= 1 - refl;
	pixelColor += reflCol * refl;
}

FloatColor Renderer::GetGlobalIllumination(const HitInfo& hitInfo, const unsigned depth)
{
	//Generate axies
	Vec3 norm = hitInfo.normal, tan, bitan;
	Math::CreateCoordSystem(norm, tan, bitan);

	FloatColor indirectColor;
	auto s = m_RenderSettings.GISampleCount;

	for (int i = 0; i < s; ++i)
	{
		//Get random sample direction
		Vec3 direction = Math::SampleHemisphere(norm, tan, bitan);
		HitInfo GIHi;
		auto hitobj = Trace(hitInfo.position, direction, GIHi, hitInfo.hitObject);
		//Get color from sample raycast
		if (hitobj != nullptr)
			indirectColor += GetHitColor(hitobj, GIHi, direction, depth + 1) * direction.Dot(norm);
	}
	//average color
	indirectColor /= s;
	return indirectColor;
}

Color Renderer::GetObjectColor(const HitInfo& hitInfo) const
{
	auto bc = hitInfo.hitObject->GetBaseColor();

	//Sample texture if there is one
	auto objTex = hitInfo.hitObject->GetTexture();
	auto texCoord = hitInfo.uvCoordinate / hitInfo.hitObject->GetMaterial().GetScale();

	if (objTex != nullptr)
	{
		bc = objTex->GetPixelColor(texCoord.x, texCoord.y);
	}

	return bc;
}

Vec3 Renderer::GetHitNormal(const HitInfo& hitInfo) const
{
	auto objNormal = hitInfo.hitObject->GetNormalMap();
	if (objNormal != nullptr)
	{
		Vec2 texCoord = hitInfo.uvCoordinate / hitInfo.hitObject->GetMaterial().GetScale();
		Color sampleColor = objNormal->GetPixelColor(texCoord.x, texCoord.y); //sample color from normal map
		Vec3 sampleNormal = Vec3(sampleColor.r / 255.f, sampleColor.g / 255.f, sampleColor.b / 255.f) * 2 - Vec3(1); //convert color to 0 to 1 range normal
		return Math::TangentToWorld(sampleNormal, hitInfo.normal); //convert normal from tangets space to world space
	}

	return hitInfo.normal;
}

Color Renderer::GetReflection(const Vec3& rayDir, const HitInfo& hitInfo, int currentDepth)
{
	//reflect incoming ray
	auto reflectedRay = Math::ReflectVector(rayDir, hitInfo.normal);
	Vec3 norm = hitInfo.normal, tan, bitan;
	Math::CreateCoordSystem(norm, tan, bitan);

	FloatColor totalCol;
	auto roughness = hitInfo.hitObject->GetRoughness();
	int rs = roughness > 0.f ? m_RenderSettings.roughnessSampleCount : 1;
	rs = rs == 0 ? 1 : rs;
	for (int i = 0; i < rs; ++i)
	{
		//Take random direction
		auto dir = Math::SampleHemisphere(norm, tan, bitan);
		//Mix random direction and perfect reflectino
		dir = dir * roughness + reflectedRay * (1 - roughness);

		//Trace new direction
		HitInfo reflHitInfo;
		auto startPos = hitInfo.position + dir * 1e-5;
		Object* reflectedObj = Trace(startPos, dir, reflHitInfo, hitInfo.hitObject); //ignore self when tracing reflection
		if (reflectedObj != nullptr && currentDepth < m_RenderSettings.maxRenderDepth)
		{
			reflHitInfo.position += dir * .1f;
			totalCol += GetHitColor(reflectedObj, reflHitInfo, dir, currentDepth + 1);
		}
	}

	//Average result
	return (totalCol / rs).ToCharColor();
}

void Renderer::ClearPixelMask()
{
	m_bDone = false;
	memset(m_PixelMask, 0, m_RenderWidth * m_RenderHeight);
}

void Renderer::ClearPixelBuffer()
{
	m_bDone = false;
	memset(m_Pixels, 0, m_RenderWidth * m_RenderHeight * sizeof(Color));
}

void Renderer::DrawText(SDL_Renderer* renderer, const char* aText, const char* aFontFile, int aX, int aY)
{
	TTF_Font* font = TTF_OpenFont(aFontFile, 24);
	if (!font)
		std::cout << "Failed to load font" << std::endl;

	SDL_Color fg = { 173,16,16,255 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, aText, fg);

	SDL_Texture* optimizedSurface = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect sizeRect;
	sizeRect.x = 0;
	sizeRect.y = 0;
	sizeRect.w = surface->w;
	sizeRect.h = surface->h;

	SDL_Rect posRect;
	posRect.x = aX;
	posRect.y = aY;
	posRect.w = sizeRect.w;
	posRect.h = sizeRect.h;

	SDL_RenderCopy(renderer, optimizedSurface, &sizeRect, &posRect);
	SDL_DestroyTexture(optimizedSurface);
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
}

int Renderer::CheckUndrawn()
{
	int size = m_RenderWidth * m_RenderHeight;
	int count = 0;
	for (int i = 0; i < size; ++i)
	{
		if (m_PixelMask[i] == 0)
		{
			++count;
			int x = i % (int)m_RenderWidth;
			int y = i / m_RenderWidth;
			RenderPixel(x, y);
		}
	}

	return count;
}