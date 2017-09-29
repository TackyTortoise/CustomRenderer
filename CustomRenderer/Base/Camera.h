#pragma once

class Camera
{
public:
	Camera(float fov, float aspectRatio) : m_Fov(fov), m_AspectRatio(aspectRatio){}

	const Vec3& GetPosition() const { return m_Position; }
	float GetFov() const { return m_Fov; }
	float GetAspectRatio() const { return m_AspectRatio; }

private:
	Vec3 m_Position = Vec3(0);
	float m_Fov = 60.f;
	float m_AspectRatio = 0.f;
};