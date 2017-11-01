#include "Light.h"

Light::Light(Vec3 pos, Vec3 normal, float width, float height, Color col) : m_Position(pos), m_Normal(normal), m_Width(width), m_Height(height), m_Color(col)
{
	//calculate light quad axes
	m_Right = Vec3(1, 0, 0);
	m_Up = Vec3(0, 0, 1);
}

Light::~Light()
{
}

Vec3 Light::GetPointInAreaLight() const
{
	//generate random point on light quad
	Vec3 p = m_Position;
	float u = Math::GetRandomFloat(-1.f, 1.f);
	p += m_Right * u * m_Width / 2.f;
	u = Math::GetRandomFloat(-1.f, 1.f);
	p += m_Up * u * m_Height / 2.f;
	return p;
}
