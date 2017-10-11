#pragma once
#include <ostream>
#include <iostream>
#include "Vec3.h"

class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4(float e00, float e01, float e02, float e03, float e10, float e11, float e12, float e13, float e20, float e21, float e22, float e23, float e30, float e31, float e32, float e33);
	~Matrix4x4();
	
	const float* operator[](unsigned i) const { return m_Values[i]; }
	float* operator[](unsigned i) { return m_Values[i]; }
	Matrix4x4 operator* (const Matrix4x4& o) const;


	float m_Values[4][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };

	friend std::ostream& operator <<(std::ostream& str, const Matrix4x4& v);

	Vec3 TransformVector(const Vec3& v) const;

	static Matrix4x4 GenerateRotation(const Vec3& angles);
	static Matrix4x4 GenerateTranslation(const Vec3& trans);
	static Matrix4x4 GenerateScale(const Vec3& scale);

	static Matrix4x4 Identity;

private:
	static Matrix4x4 GenerateXRotation(float angle);
	static Matrix4x4 GenerateYRotation(float angle);
	static Matrix4x4 GenerateZRotation(float angle);
};

