#pragma once

#include <cmath>
#include "Vector3.h"

class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4(const Matrix4x4& matrix);
	Matrix4x4
		(float pm11, float pm12, float pm13, float pm14,
		float pm21, float pm22, float pm23, float pm24, 
		float pm31, float pm32, float pm33, float pm34, 
		float pm41, float pm42, float pm43, float pm44);

	~Matrix4x4();

	void lookAtLh(const Vector3& position, const Vector3& target, const Vector3& up);

	void identity();
	void zero();
	Matrix4x4 lerp(const Matrix4x4& matrix, float t) const;

	void orthoProjectionLhD3d(float width, float height, float zNear, float zFar);
	void orthoProjectionRhD3d(float width, float height, float zNear, float zFar);

	void perspectiveProjectionFovLhD3d(float fovY, float aspectRatio, float zNear, float zFar);
	void perspectiveProjectionFovRhD3d(float fovY, float aspectRatio, float zNear, float zFar);

	void orthoProjectionLhOgl(float width, float height, float zNear, float zFar);
	void orthoProjectionRhOgl(float width, float height, float zNear, float zFar);

	void perspectiveProjectionFovLhOgl(float fovY, float aspectRatio, float zNear, float zFar);
	void perspectiveProjectionFovRhOgl(float fovY, float aspectRatio, float zNear, float zFar);

	void transpose();
	Matrix4x4 transposition() const;

	Matrix4x4& invert();
	Matrix4x4 inverse() const;

	float determinant() const;

	Matrix4x4& operator = (const Matrix4x4& matrix);

	Matrix4x4 operator * (const Matrix4x4& matrix) const;
	Matrix4x4 operator + (const Matrix4x4& matrix) const;
	Matrix4x4 operator - (const Matrix4x4& matrix) const;

	Matrix4x4& operator *= (const Matrix4x4& matrix);
	Matrix4x4& operator += (const Matrix4x4& matrix);
	Matrix4x4& operator -= (const Matrix4x4& matrix);

	Matrix4x4 operator * (float scalar) const;
	Matrix4x4 operator / (float scalar) const;

	Matrix4x4 operator + (float scalar) const;
	Matrix4x4 operator - (float scalar) const;

	Matrix4x4& operator *= (float scalar);
	Matrix4x4& operator /= (float scalar);

	Matrix4x4& operator += (float scalar);
	Matrix4x4& operator -= (float scalar);

	union
	{
		struct
		{
			float m11, m12, m13, m14;
			float m21, m22, m23, m24;
			float m31, m32, m33, m34;
			float m41, m42, m43, m44;
		};

		float m[16];
	};
};

inline void Matrix4x4::lookAtLh(const Vector3& position, const Vector3& target, const Vector3& up)
{
	Vector3 zAxis = (target - position);
	zAxis.normalize();

	Vector3 xAxis = up.crossProduct(zAxis);
	xAxis.normalize();

	Vector3 yAxis = zAxis.crossProduct(xAxis);

	m11 = xAxis.x;
	m21 = xAxis.y;
	m31 = xAxis.z;
	m41 = -xAxis.dotProduct(position);

	m12 = yAxis.x;
	m22 = yAxis.y;
	m32 = yAxis.z;
	m42 = -yAxis.dotProduct(position);

	m13 = zAxis.x;
	m23 = zAxis.y;
	m33 = zAxis.z;
	m43 = -zAxis.dotProduct(position);

	m14 = 0.0f;
	m24 = 0.0f;
	m34 = 0.0f;
	m44 = 1.0f;
}

inline void Matrix4x4::identity()
{
	m11 = m22 = m33 = m44 = 1.0f;

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
			m34 = m41 = m42 = m43 = 0.0f;
}

inline void Matrix4x4::zero()
{
	m11 = m12 = m13 = m14 =
		m21 = m22 = m23 = m24 =
			m31 = m32 = m33 = m34 =
				m41 = m42 = m43 = m44 = 0.0f;
}

inline Matrix4x4 Matrix4x4::lerp(const Matrix4x4& matrix, float t) const
{
	return Matrix4x4
		((matrix.m11 - m11) * t + m11, (matrix.m12 - m12) * t + m12, (matrix.m13 - m13) * t + m13, (matrix.m14 - m14) * t + m14,
		(matrix.m21 - m21) * t + m21, (matrix.m22 - m22) * t + m22, (matrix.m23 - m23) * t + m23, (matrix.m24 - m24) * t + m24,
		(matrix.m31 - m31) * t + m31, (matrix.m32 - m32) * t + m32, (matrix.m33 - m33) * t + m33, (matrix.m34 - m34) * t + m34,
		(matrix.m41 - m41) * t + m41, (matrix.m42 - m42) * t + m42, (matrix.m43 - m43) * t + m43, (matrix.m44 - m44) * t + m44);
}

inline void Matrix4x4::orthoProjectionLhD3d(float width, float height, float zNear, float zFar)
{
	m11 = 2.0f / width;
	m22 = 2.0f / height;
	m33 = 1.0f / (zFar - zNear);
	m43 = zNear / (zNear - zFar);
	m44 = 1.0;

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
		m34 = m41 = m42 = 0.0f;
}

inline void Matrix4x4::orthoProjectionRhD3d(float width, float height, float zNear, float zFar)
{
	m11 = 2.0f / width;
	m22 = 2.0f / height;
	m33 = 1.0f / (zNear - zFar);
	m43 = zNear / (zNear - zFar);
	m44 = 1.0;

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
		m34 = m41 = m42 = 0.0f;
}

inline void Matrix4x4::perspectiveProjectionFovLhD3d(float fovY, float aspectRatio, float zNear, float zFar)
{
	float cotangent = 1.0f / tanf(fovY / 2.0f);

	m11 = cotangent / aspectRatio;
	m22 = cotangent;
	m33 = zFar / (zFar - zNear);
	m34 = 1.0f;
	m43 = -zNear * zFar / (zFar - zNear);

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
			m41 = m42 = m44 = 0.0f;
}

inline void Matrix4x4::perspectiveProjectionFovRhD3d(float fovY, float aspectRatio, float zNear, float zFar)
{
	float cotangent = 1.0f / tanf(fovY / 2.0f);

	m11 = cotangent / aspectRatio;
	m22 = cotangent;
	m33 = zFar / (zNear - zFar);
	m34 = -1.0f;
	m43 = zNear * zFar / (zNear - zFar);

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
			m41 = m42 = m44 = 0.0f;
}

inline void Matrix4x4::orthoProjectionLhOgl(float width, float height, float zNear, float zFar)
{
	m11 = 2.0f / width;
	m22 = 2.0f / height;
	m33 = 2.0f / (zFar - zNear);
	m43 = (zNear + zFar) / (zNear - zFar);
	m44 = 1.0;

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
			m34 = m41 = m42 = 0.0f;
}

inline void Matrix4x4::orthoProjectionRhOgl(float width, float height, float zNear, float zFar)
{
	m11 = 2.0f / width;
	m22 = 2.0f / height;
	m33 = 2.0f / (zNear - zFar);
	m43 = (zNear + zFar) / (zNear - zFar);
	m44 = 1.0;

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
			m34 = m41 = m42 = 0.0f;
}

inline void Matrix4x4::perspectiveProjectionFovLhOgl(float fovY, float aspectRatio, float zNear, float zFar)
{
	float cotangent = 1.0f / tanf(fovY / 2.0f);

	m11 = cotangent / aspectRatio;
	m22 = cotangent;
	m33 = (zNear + zFar) / (zFar - zNear);
	m34 = 1.0;
	m43 = -2.0f * zNear * zFar / (zFar - zNear);

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
			m41 = m42 = m44 = 0.0f;
}

inline void Matrix4x4::perspectiveProjectionFovRhOgl(float fovY, float aspectRatio, float zNear, float zFar)
{
	float cotangent = 1.0f / tanf(fovY / 2.0f);

	m11 = cotangent / aspectRatio;
	m22 = cotangent;
	m33 = (zNear + zFar) / (zNear - zFar);
	m34 = -1.0;
	m43 = 2.0f * zNear * zFar / (zNear - zFar);

	m12 = m13 = m14 = m21 =
		m23 = m24 = m31 = m32 =
			m41 = m42 = m44 = 0.0f;
}

inline void Matrix4x4::transpose()
{
	float lv12 = m12;
	float lv13 = m13;
	float lv14 = m14;
	float lv23 = m23;
	float lv24 = m24;
	float lv34 = m34;

	m12 = m21;
	m13 = m31;
	m14 = m41;
	m23 = m32;
	m24 = m42;
	m34 = m43;

	m21 = lv12;
	m31 = lv13;
	m41 = lv14;
	m32 = lv23;
	m42 = lv24;
	m43 = lv34;
}

inline Matrix4x4 Matrix4x4::transposition() const
{
	return Matrix4x4
		(m11, m21, m31, m41,
		m12, m22, m32, m42,
		m13, m23, m33, m43,
		m14, m24, m34, m44);
}

inline Matrix4x4& Matrix4x4::operator = (const Matrix4x4& matrix)
{
	m11 = matrix.m11; m12 = matrix.m12; m13 = matrix.m13; m14 = matrix.m14;
	m21 = matrix.m21; m22 = matrix.m22; m23 = matrix.m23; m24 = matrix.m24;
	m31 = matrix.m31; m32 = matrix.m32; m33 = matrix.m33; m34 = matrix.m34;
	m41 = matrix.m41; m42 = matrix.m42; m43 = matrix.m43; m44 = matrix.m44;

	return *this;
}

inline Matrix4x4 Matrix4x4::operator + (const Matrix4x4& matrix) const
{
	return Matrix4x4
		(m11 + matrix.m11, m12 + matrix.m12, m13 + matrix.m13, m14 + matrix.m14,
		m21 + matrix.m21, m22 + matrix.m22, m23 + matrix.m23, m24 + matrix.m24,
		m31 + matrix.m31, m32 + matrix.m32, m33 + matrix.m33, m34 + matrix.m34,
		m41 + matrix.m41, m42 + matrix.m42, m43 + matrix.m43, m44 + matrix.m44);
}

inline Matrix4x4 Matrix4x4::operator - (const Matrix4x4& matrix) const
{
	return Matrix4x4
		(m11 - matrix.m11, m12 - matrix.m12, m13 - matrix.m13, m14 - matrix.m14,
		m21 - matrix.m21, m22 - matrix.m22, m23 - matrix.m23, m24 - matrix.m24,
		m31 - matrix.m31, m32 - matrix.m32, m33 - matrix.m33, m34 - matrix.m34,
		m41 - matrix.m41, m42 - matrix.m42, m43 - matrix.m43, m44 - matrix.m44);
}

inline Matrix4x4& Matrix4x4::operator += (const Matrix4x4& matrix)
{
	m11 += matrix.m11; m12 += matrix.m12; m13 += matrix.m13; m14 += matrix.m14;
	m21 += matrix.m21; m22 += matrix.m22; m23 += matrix.m23; m24 += matrix.m24;
	m31 += matrix.m31; m32 += matrix.m32; m33 += matrix.m33; m34 += matrix.m34;
	m41 += matrix.m41; m42 += matrix.m42; m43 += matrix.m43; m44 += matrix.m44;

	return *this;
}

inline Matrix4x4& Matrix4x4::operator -= (const Matrix4x4& matrix)
{
	m11 -= matrix.m11; m12 -= matrix.m12; m13 -= matrix.m13; m14 -= matrix.m14;
	m21 -= matrix.m21; m22 -= matrix.m22; m23 -= matrix.m23; m24 -= matrix.m24;
	m31 -= matrix.m31; m32 -= matrix.m32; m33 -= matrix.m33; m34 -= matrix.m34;
	m41 -= matrix.m41; m42 -= matrix.m42; m43 -= matrix.m43; m44 -= matrix.m44;

	return *this;
}

inline Matrix4x4 Matrix4x4::operator * (float scalar) const
{
	return Matrix4x4
		(m11 * scalar, m12 * scalar, m13 * scalar, m14 * scalar,
		m21 * scalar, m22 * scalar, m23 * scalar, m24 * scalar,
		m31 * scalar, m32 * scalar, m33 * scalar, m34 * scalar,
		m41 * scalar, m42 * scalar, m43 * scalar, m44 * scalar);
}

inline Matrix4x4 Matrix4x4::operator / (float scalar) const
{
	return Matrix4x4
		(m11 / scalar, m12 / scalar, m13 / scalar, m14 / scalar,
		m21 / scalar, m22 / scalar, m23 / scalar, m24 / scalar,
		m31 / scalar, m32 / scalar, m33 / scalar, m34 / scalar,
		m41 / scalar, m42 / scalar, m43 / scalar, m44 / scalar);
}

inline Matrix4x4 Matrix4x4::operator + (float scalar) const
{
	return Matrix4x4
		(m11 + scalar, m12 + scalar, m13 + scalar, m14 + scalar,
		m21 + scalar, m22 + scalar, m23 + scalar, m24 + scalar,
		m31 + scalar, m32 + scalar, m33 + scalar, m34 + scalar,
		m41 + scalar, m42 + scalar, m43 + scalar, m44 + scalar);
}

inline Matrix4x4 Matrix4x4::operator - (float scalar) const
{
	return Matrix4x4
		(m11 - scalar, m12 - scalar, m13 - scalar, m14 - scalar,
		m21 - scalar, m22 - scalar, m23 - scalar, m24 - scalar,
		m31 - scalar, m32 - scalar, m33 - scalar, m34 - scalar,
		m41 - scalar, m42 - scalar, m43 - scalar, m44 - scalar);
}

inline Matrix4x4& Matrix4x4::operator *= (float scalar)
{
	m11 *= scalar; m12 *= scalar; m13 *= scalar; m14 *= scalar;
	m21 *= scalar; m22 *= scalar; m23 *= scalar; m24 *= scalar;
	m31 *= scalar; m32 *= scalar; m33 *= scalar; m34 *= scalar;
	m41 *= scalar; m42 *= scalar; m43 *= scalar; m44 *= scalar;

	return *this;
}

inline Matrix4x4& Matrix4x4::operator /= (float scalar)
{
	m11 /= scalar; m12 /= scalar; m13 /= scalar; m14 /= scalar;
	m21 /= scalar; m22 /= scalar; m23 /= scalar; m24 /= scalar;
	m31 /= scalar; m32 /= scalar; m33 /= scalar; m34 /= scalar;
	m41 /= scalar; m42 /= scalar; m43 /= scalar; m44 /= scalar;

	return *this;
}

inline Matrix4x4& Matrix4x4::operator += (float scalar)
{
	m11 += scalar; m12 += scalar; m13 += scalar; m14 += scalar;
	m21 += scalar; m22 += scalar; m23 += scalar; m24 += scalar;
	m31 += scalar; m32 += scalar; m33 += scalar; m34 += scalar;
	m41 += scalar; m42 += scalar; m43 += scalar; m44 += scalar;

	return *this;
}

inline Matrix4x4& Matrix4x4::operator -= (float scalar)
{
	m11 -= scalar; m12 -= scalar; m13 -= scalar; m14 -= scalar;
	m21 -= scalar; m22 -= scalar; m23 -= scalar; m24 -= scalar;
	m31 -= scalar; m32 -= scalar; m33 -= scalar; m34 -= scalar;
	m41 -= scalar; m42 -= scalar; m43 -= scalar; m44 -= scalar;

	return *this;
}
