#include "Camera.h"

Camera::Camera(float fov, float aspectRatio) : m_Fov(fov), m_AspectRatio(aspectRatio)
{
	m_CamTan = tan(fov / 2 * M_PI / 180.f);
	m_CamTransform = new Transform();
	GetRight(true);
	GetUp(true);
}

Camera::~Camera()
{
	if (m_CamTransform)
	{
		delete m_CamTransform;
		m_CamTransform = nullptr;
	}
}

Vec3 Camera::GetCameraRay(int pixelX, int pixelY, float renderWidth, float renderHeight, bool randomOffset) const
{
	//transform pixel to camera space
	float cX, cY;
	if (!randomOffset)
	{
		//Get center of pixel
		cX = (2 * ((pixelX + .5f) / renderWidth) - 1) * m_CamTan * m_AspectRatio;
		cY = (1 - 2 * ((pixelY + .5f) / renderHeight)) * m_CamTan;
	}
	else
	{
		//Get random position in pixel
		auto xRand = Math::GetRandomFloat();
		auto yRand = Math::GetRandomFloat();
		cX = (2 * ((pixelX + xRand) / renderWidth) - 1) * m_CamTan * m_AspectRatio;
		cY = (1 - 2 * ((pixelY + yRand) / renderHeight)) * m_CamTan;
	}

	//create ray from camera to pixel
	Vec3 rayDir = Vec3(cX, cY, 1.f);// -Vec3::zero; //start from camera position 0,0,0

	//multiply with camtoworld rotation if cam is not in 0 rotation transform
		rayDir = m_CamTransform->GetRotationMatrix().TransformVector(rayDir);

	rayDir.Normalize();
	return rayDir;
}

Vec3 Camera::GetCameraRayDOF(int pixelX, int pixelY, float renderWidth, float renderHeight, bool randomOffset)
{
	//Get normal ray
	Vec3 ray = GetCameraRay(pixelX, pixelY, renderWidth, renderHeight, randomOffset);

	//Stop if camera has no DOF for scene
	if (!m_bEnableDOF)
		return ray;

	//Calculate focuspoint
	Vec3 focusPoint = ray * m_FocalDistance;

	//Offset ray start position
	auto diskPoint = Math::SampleDisk() * m_Aperture;

	//Vec3 newOrigin(diskPoint.x, diskPoint.y, 0.f);
	Vec3 newOrigin = diskPoint.x * m_Right + diskPoint.y * m_Up;
	m_DofPosOffset = newOrigin;

	//Calculate new ray
	Vec3 newRay = (focusPoint - newOrigin).Normalized();

	return newRay;
}

Vec3 Camera::GetRight(bool dirty)
{
	if (!dirty)
		return m_Right;

	//Update right vector if dirty (after rotation)
	Vec3 right(1, 0, 0);
	m_Right = m_CamTransform->GetRotationMatrix().TransformVector(right);
	return m_Right;
}

Vec3 Camera::GetUp(bool dirty)
{
	if (!dirty)
		return m_Up;

	//Update up vector if dirty (after rotation)
	Vec3 up(0, 1, 0);
	m_Up = m_CamTransform->GetRotationMatrix().TransformVector(up);
	return m_Up;
}
