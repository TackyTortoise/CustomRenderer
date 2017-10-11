#pragma once
#include <SDL_stdinc.h>
#include "../Math/Math.h"

class Camera
{
public:
	Camera(float fov, float aspectRatio);
	~Camera();
	const Vec3& GetPosition() const { return m_CamTransform->GetPosition(); }
	float GetFov() const { return m_Fov; }
	float GetAspectRatio() const { return m_AspectRatio; }
	Vec3 GetCameraRay(int pixelX, int pixelY, float renderWidth, float renderHeight);

	Transform* GetTransform() { return  m_CamTransform; }

	static Transform* m_CamTransform;
private:
	float m_Fov = 60.f;
	float m_AspectRatio = 0.f;
	float m_CamTan = 0.f;

};
