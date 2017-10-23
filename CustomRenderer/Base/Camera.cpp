#include "Camera.h"

Camera::Camera(float fov, float aspectRatio) : m_Fov(fov), m_AspectRatio(aspectRatio)
{
	m_CamTan = tan(fov / 2 * M_PI / 180.f);
	m_CamTransform = new Transform();
}

Camera::~Camera()
{
	if (m_CamTransform)
	{
		delete m_CamTransform;
		m_CamTransform = nullptr;
	}
}

Vec3 Camera::GetCameraRay(int pixelX, int pixelY, float renderWidth, float renderHeight, bool random) const
{
	//transform pixel to camera space
	float cX, cY;
	if (!random)
	{
		cX = (2 * ((pixelX + .5f) / renderWidth) - 1) * m_CamTan * m_AspectRatio;
		cY = (1 - 2 * ((pixelY + .5f) / renderHeight)) * m_CamTan;
	}
	else
	{
		auto xRand = Math::GetRandomFloat();
		auto yRand = Math::GetRandomFloat();
		cX = (2 * ((pixelX + xRand) / renderWidth) - 1) * m_CamTan * m_AspectRatio;
		cY = (1 - 2 * ((pixelY + yRand) / renderHeight)) * m_CamTan;
	}

	//create ray from camera to pixel
	Vec3 rayDir = Vec3(cX, cY, 1.f) - Vec3::zero; //start from camera position 0,0,0

	//multiply with camtoworld rotation if cam is not in 0 transform
	rayDir = m_CamTransform->GetRotationMatrix().TransformVector(rayDir);
	rayDir.Normalize();
	return rayDir;
}
