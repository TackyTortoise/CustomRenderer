#include "Matrix4x4.h"

Matrix4x4 Matrix4x4::Identity = Matrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

Matrix4x4::Matrix4x4()
{
}

Matrix4x4::Matrix4x4(float e00, float e01, float e02, float e03, float e10, float e11, float e12, float e13, float e20, float e21, float e22, float e23, float e30, float e31, float e32, float e33)
{
	//Initialize values
	m_Values[0][0] = e00;
	m_Values[0][1] = e01;
	m_Values[0][2] = e02;
	m_Values[0][3] = e03;

	m_Values[1][0] = e10;
	m_Values[1][1] = e11;
	m_Values[1][2] = e12;
	m_Values[1][3] = e13;

	m_Values[2][0] = e20;
	m_Values[2][1] = e21;
	m_Values[2][2] = e22;
	m_Values[2][3] = e23;

	m_Values[3][0] = e30;
	m_Values[3][1] = e31;
	m_Values[3][2] = e32;
	m_Values[3][3] = e33;
}

Matrix4x4::~Matrix4x4(){}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& o) const
{
	Matrix4x4 r;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			r[i][j] = m_Values[i][0] * o[0][j] +
				m_Values[i][1] * o[1][j] +
				m_Values[i][2] * o[2][j] +
				m_Values[i][3] * o[3][j];
		}
	}
	return r;
}

Vec3 Matrix4x4::TransformVector(const Vec3& v) const
{
	Vec3 r;
	r.x = v.x * m_Values[0][0] + v.y * m_Values[1][0] + v.z * m_Values[2][0] + m_Values[0][3];
	r.y = v.x * m_Values[0][1] + v.y * m_Values[1][1] + v.z * m_Values[2][1] + m_Values[1][3];
	r.z = v.x * m_Values[0][2] + v.y * m_Values[1][2] + v.z * m_Values[2][2] + m_Values[2][3];

	//perspective division if necessary - ignored as it is nog needed
	/*auto w = v.x * m_Values[0][3] + v.y * m_Values[1][3] + v.z * m_Values[2][3] + m_Values[3][3];
	if (w != 1 && w != 0)
	{
		r.x /= w;
		r.y /= w;
		r.z /= w;
	}*/
	return r;
}

Matrix4x4 Matrix4x4::GenerateRotation(const Vec3& angles)
{
	Matrix4x4 x = GenerateXRotation(angles.x);
	Matrix4x4 y = GenerateYRotation(angles.y);
	Matrix4x4 z = GenerateZRotation(angles.z);

	return x * y * z;
}

Matrix4x4 Matrix4x4::GenerateTranslation(const Vec3& trans)
{
	Matrix4x4 r = Identity;
	r[0][3] = trans.x;
	r[1][3] = trans.y;
	r[2][3] = trans.z;
	return r;
}

Matrix4x4 Matrix4x4::GenerateScale(const Vec3& scale)
{
	Matrix4x4 s = Identity;
	s[0][0] = scale.x;
	s[1][1] = scale.y;
	s[2][2] = scale.z;
	return s;
}

Matrix4x4 Matrix4x4::GenerateXRotation(float angle) 
{
	return Matrix4x4(1, 0,          0,          0,
					0,  cos(angle), sin(angle), 0,
					0, -sin(angle), cos(angle), 0,
					0,  0,          0,          1);
}

Matrix4x4 Matrix4x4::GenerateYRotation(float angle)
{
	return Matrix4x4(
		cos(angle), 0, -sin(angle), 0,
		0,          1,  0,          0,
		sin(angle), 0,  cos(angle), 0,
		0,          0,  0,          1
	);
}

Matrix4x4 Matrix4x4::GenerateZRotation(float angle)
{
	return Matrix4x4(
		cos(angle), sin(angle), 0, 0,
		-sin(angle),cos(angle), 0, 0,
		0,          0,          1, 0,
		0,          0,          0, 1
	);
}

std::ostream& operator<<(std::ostream& str, const Matrix4x4& v)
{
	str << "[ " << v.m_Values[0][0] << ", " << v.m_Values[0][1] << ", " << v.m_Values[0][2] << ", " << v.m_Values[0][3] << " ]" << std::endl;
	str << "[ " << v.m_Values[1][0] << ", " << v.m_Values[1][1] << ", " << v.m_Values[1][2] << ", " << v.m_Values[1][3] << " ]" << std::endl;
	str << "[ " << v.m_Values[2][0] << ", " << v.m_Values[2][1] << ", " << v.m_Values[2][2] << ", " << v.m_Values[2][3] << " ]" << std::endl;
	str << "[ " << v.m_Values[3][0] << ", " << v.m_Values[3][1] << ", " << v.m_Values[3][2] << ", " << v.m_Values[3][3] << " ]";
	return str;
}
