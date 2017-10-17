#include "Transform.h"

Transform::Transform()
{
}


Transform::~Transform()
{
}

void Transform::SetFullTransform(const Vec3& pos, const Vec3& rot, const Vec3& scale)
{
	m_Position = pos;
	m_Angles = rot;
	m_Scale = scale;

	m_mTranslation = Matrix4x4::GenerateTranslation(m_Position);
	m_mRotation = Matrix4x4::GenerateRotation(m_Angles);
	m_mScale = Matrix4x4::GenerateScale(m_Scale);

	m_mTransform = m_mTranslation * m_mRotation * m_mScale;
}

void Transform::SetTranslation(const Vec3& newPos)
{
	m_Position = newPos;
	m_mTranslation = Matrix4x4::GenerateTranslation(m_Position);
	m_mTransform = m_mTranslation * m_mRotation * m_mScale;
}

void Transform::SetRotation(const Vec3& newAngles)
{
	m_Angles = newAngles;
	m_mRotation = Matrix4x4::GenerateRotation(m_Angles);
	m_mTransform = m_mTranslation * m_mRotation * m_mScale;
}

void Transform::SetScale(const Vec3& newScale)
{
	m_Scale = newScale;
	m_mScale = Matrix4x4::GenerateScale(m_Scale);
	m_mTransform = m_mTranslation * m_mRotation * m_mScale;
}

void Transform::Move(const Vec3& movement)
{
	m_Position += movement;
	m_mTranslation = Matrix4x4::GenerateTranslation(m_Position);
	m_mTransform = m_mTranslation * m_mRotation * m_mScale;
}

void Transform::Rotate(const Vec3& rotation)
{
	m_Angles += rotation;
	m_mRotation = Matrix4x4::GenerateRotation(m_Angles);
	m_mTransform = m_mTranslation * m_mRotation * m_mScale;
}

