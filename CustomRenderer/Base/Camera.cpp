#include "Camera.h"

Transform* Camera::m_CamTransform = new Transform();

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

Vec3 Camera::GetCameraRay(int pixelX, int pixelY, float renderWidth, float renderHeight)
{
	//transform pixel to camera space
	float cX = (2 * ((pixelX + .5f) / renderWidth) - 1) * m_CamTan * m_AspectRatio;
	float cY = (1 - 2 * ((pixelY + .5f) / renderHeight)) * m_CamTan;

	//create ray from camera to pixel
	Vec3 rayOrg(0); //start from camera position 0,0,0
	Vec3 rayDir = Vec3(cX, cY, 1.f) - rayOrg;
	//multiply with camtoworld if cam is not in 0 transform
	//rayDir = m_CamTransform.GetRotationMatrix().TransformVector(rayDir);
	rayDir.Normalize();
	return rayDir;
}
