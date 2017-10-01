#include "Renderer.h"
#include <iostream>
#include <SDL_stdinc.h>

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
	SetBlockCount(blockCount);
}

void Renderer::Close()
{
	if (m_Pixels != nullptr)
	{
		delete m_Pixels;
		m_Pixels = nullptr;
	}
}

void Renderer::SetBlockCount(const int bc)
{
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
	m_RenderObjects = &scene->GetObjectPointer()[0];
	m_ObjectCount = scene->GetObjectPointer().size();
}

void Renderer::RenderScene()
{
	auto sceneCam = m_ActiveScene->GetCamera();
	float camAngleTan = tan(sceneCam->GetFov() / 2 * M_PI / 180.f);
	float ar = sceneCam->GetAspectRatio();

	for (int x = 0; x < m_RenderWidth - m_RegionSize.x + 1; x += m_RegionSize.x)
	{
		for (int y = 0; y < m_RenderHeight - m_RegionSize.y + 1; y += m_RegionSize.y)
		{
			//choose random pixel within block
			float rX;
			do
			{
				rX = x + rand() % static_cast<int>(m_RegionSize.x);
			} while (rX >= m_RenderWidth);

			float rY;
			do
			{
				rY = y + rand() % static_cast<int>(m_RegionSize.y);
			} while (rY >= m_RenderHeight);

			//Generate ray from camera to pixel
			Vec3 rayDir = CalculateCameraRay(rX, rY, camAngleTan, ar);

			float hitDistance = -1.f;
			Vec3 orgHitPoint, orgHitNormal;
			Object* closestObj = Trace(sceneCam->GetPosition(), rayDir, hitDistance, orgHitPoint, orgHitNormal);

			int pixelIndex = rX + m_RenderWidth * rY;
			//On object hit Color it
			if (closestObj != nullptr)
			{
				Color pixelColor = closestObj->GetBaseColor();
				auto light = m_ActiveScene->GetLights()[0];
				Vec3 toLight = (light - orgHitPoint).Normalized();

				//Reflection
				float refl = closestObj->GetReflective();
				if (refl > 0.f)
				{
					auto invRay = -rayDir;
					auto reflectedRay = invRay.ReflectAround(orgHitNormal);
					Vec3 reflHit, reflNorm;
					Object* reflectedObj = Trace(orgHitPoint, reflectedRay, hitDistance, reflHit, reflNorm, closestObj);
					if (reflectedObj != nullptr)
					{
						pixelColor *= 1 - refl;
						Color reflCol = reflectedObj->GetBaseColor();
						pixelColor += reflCol * refl;
					}
				}

				//Transparancy
				float transp = closestObj->GetTransparancy();
				if (transp > 0)
				{
					Vec3 transHit, transNorm;
					Object* transObj = Trace(orgHitPoint, rayDir, hitDistance, transHit, transNorm, closestObj);
					//maybe this should go in if?
					pixelColor *= 1 - transp;
					if (transObj != nullptr)
					{
						//pixelColor *= 1 - transp;
						Color transCol = transObj->GetBaseColor();
						pixelColor += transCol * transp;
					}
				}

				//shadow
				Vec3 sh, sn;
				auto csb = Trace(orgHitPoint, toLight, hitDistance,sh, sn, closestObj, true);
				if (csb != nullptr)
					pixelColor *= 1 - m_ShadowIntensity;

				//Diffuse color
				float diffuseIntensity = Math::CalculateDiffuseIntensity(toLight, -rayDir, orgHitNormal, .15f);
				pixelColor *= diffuseIntensity;

				//specular
				auto halfVec = (toLight + -rayDir).Normalize();
				float specStrength = Math::Clamp(orgHitNormal.Dot(halfVec));
				specStrength = pow(specStrength, 50);
				Color specColor = Color(255) * specStrength;
				pixelColor = pixelColor.ClampAdd(specColor);

				m_Pixels[pixelIndex] = pixelColor;
			}
			//On miss clear pixel
			else
				m_Pixels[pixelIndex] = m_ClearColor;
		}
	}
}

Object* Renderer::Trace(const Vec3& rayOrg, const Vec3& rayDir, float& distance, Vec3& hitPoint, Vec3& hitNormal, Object* ignoreObject, bool keepIgnoreDistance)
{
	float d = 0;
	float shortD = std::numeric_limits<float>::max();
	Object* closeObject = nullptr;
	for (int i = 0; i < m_ObjectCount; ++i)
	{
		if (m_RenderObjects[i]->isHit(rayOrg, rayDir, d) && d < shortD)
		{
			if (keepIgnoreDistance)
			{
				shortD = d;
			}
			if (m_RenderObjects[i] != ignoreObject)
			{
				shortD = d;
				closeObject = m_RenderObjects[i];
			}
		}
	}

	distance = shortD;
	if (closeObject != nullptr)
	{
		hitPoint = rayOrg + rayDir * shortD;
		hitNormal = closeObject->GetNormalOnHit(hitPoint);
	}
	return closeObject;
}

const Vec3 Renderer::CalculateCameraRay(float x, float y, float camTan, float aspectRatio) const
{
	//transform pixel to camera space
	float cX = (2 * ((x + .5f) / m_RenderWidth) - 1) * camTan * aspectRatio;
	float cY = (1 - 2 * ((y + .5f) / m_RenderHeight)) * camTan;

	//create ray from camera to pixel
	Vec3 rayOrg(0); //start from camera position 0,0,0
	Vec3 rayDir = Vec3(cX, cY, -1.f) - rayOrg;
	//multiply with camtoworld if cam is not in 0 transform
	rayDir.Normalize();
	return rayDir;
}
