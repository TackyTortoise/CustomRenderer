#include "Light.h"

Light::Light(Vec3 pos, Vec3 normal, float width, float height): m_Position(pos), m_Normal(normal), m_Width(width), m_Height(height)
{
	m_Right = Vec3(1, 0, 0);
	m_Up = Vec3(0, 0, 1);
}

Light::~Light()
{
}

Vec3 Light::GetPointInAreaLight() const
{
	Vec3 p = m_Position;
	float u = -1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2);
	p += m_Right * u * m_Width;
	u = -1 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2);
	p += m_Up * u * m_Height;
	return p;
}
