#pragma once

#include "../Math/Math.h"

class Light
{
public:
	Light(Vec3 pos, Vec3 normal, float width, float height);
	~Light();

	Vec3 GetPosition() const { return m_Position; }
	Vec3 GetPointInAreaLight() const;

private:
	Vec3 m_Position, m_Normal, m_Right, m_Up;
	float m_Width, m_Height;
};

