#pragma once
#include <SDL_stdinc.h>
#include "../Math/Math.h"

class Camera
{
public:
	Camera(float fov, float aspectRatio);
	~Camera();
	const Vec3& GetPosition() const { return m_CamTransform->GetPosition() + m_DofPosOffset; }
	float GetFov() const { return m_Fov; }
	float GetAspectRatio() const { return m_AspectRatio; }
	Vec3 GetCameraRay(int pixelX, int pixelY, float renderWidth, float renderHeight, bool randomOffset = false) const;
	Vec3 GetCameraRayDOF(int pixelX, int pixelY, float renderWidth, float renderHeight, bool randomOffset = false);

	void SetFocalDistance(float v) { m_FocalDistance = v; }
	void SetAperture(float v) { m_Aperture = v; }

	void EnableDOF(bool v) { m_bEnableDOF = v; }
	bool IsDOFEnabled() const { return m_bEnableDOF; }

	Transform* GetTransform() const { return  m_CamTransform; }

	Vec3 GetRight(bool dirty = false);
	Vec3 GetUp(bool dirty = false);

private:
	float m_Fov = 60.f;
	float m_AspectRatio = 0.f;
	float m_CamTan = 0.f;

	Vec3 m_Right;
	Vec3 m_Up;

	bool m_bEnableDOF = false;
	float m_FocalDistance = 10.f;
	float m_Aperture = .07f;
	Vec3 m_DofPosOffset;

	Transform* m_CamTransform;
};
