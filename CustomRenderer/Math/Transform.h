#pragma once
#include "Vec3.h"
#include "Matrix4x4.h"

class Transform
{
public:
	Transform();
	~Transform();

	void SetFullTransform(const Vec3& pos, const Vec3& rot, const Vec3& scale);

	void SetTranslation(const Vec3& newPos);
	void SetRotation(const Vec3& newAngles);
	void SetScale(const Vec3& newScale);

	Matrix4x4& GetTranslationMatrix()  { return m_mTranslation; }
	Matrix4x4& GetRotationMatrix()  { return m_mRotation; }
	Matrix4x4& GetScaleMatrix()  { return m_mScale; }
	Matrix4x4& GetTransformation() { return m_mTransform; }

	Vec3 GetPosition() const { return m_Position; }
	Vec3 GetRotation() const { return m_Angles; }
	Vec3 GetScale() const { return m_Scale; }

private:
	Matrix4x4 m_mTranslation, m_mRotation, m_mScale, m_mTransform;
	Vec3 m_Position, m_Angles, m_Scale;
};

