#pragma once

#include "../Math/Math.h"
#include "../Math/Color.h"

class Light
{
public:
	Light(Vec3 pos, Vec3 normal, float width, float height);
	~Light();

	Vec3 GetPosition() const { return m_Position; }
	Vec3 GetPointInAreaLight() const;
	Color GetColor() const { return m_Color; }

private:
	Vec3 m_Position, m_Normal, m_Right, m_Up;
	Color m_Color = Color(255);
	float m_Width, m_Height;
};

