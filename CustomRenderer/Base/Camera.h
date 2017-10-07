#pragma once
#include <SDL_stdinc.h>

class Camera
{
public:
	Camera(float fov, float aspectRatio) : m_Fov(fov), m_AspectRatio(aspectRatio)
	{
		m_CamTan = tan(fov / 2 * M_PI / 180.f);
	}

	const Vec3& GetPosition() const { return m_Position; }
	float GetFov() const { return m_Fov; }
	float GetAspectRatio() const { return m_AspectRatio; }
	Vec3 GetCameraRay(int pixelX, int pixelY, float renderWidth, float renderHeight) const
	{
		//transform pixel to camera space
		float cX = (2 * ((pixelX + .5f) / renderWidth) - 1) * m_CamTan * m_AspectRatio;
		float cY = (1 - 2 * ((pixelY + .5f) / renderHeight)) * m_CamTan;

		//create ray from camera to pixel
		Vec3 rayOrg(0); //start from camera position 0,0,0
		Vec3 rayDir = Vec3(cX, cY, 1.f) - rayOrg;
		//multiply with camtoworld if cam is not in 0 transform
		rayDir.Normalize();
		return rayDir;
	}

private:
	Vec3 m_Position = Vec3::zero;
	float m_Fov = 60.f;
	float m_AspectRatio = 0.f;
	float m_CamTan = 0.f;
};
