#pragma once

#include <cmath>
#include "Matrix4x4.h"

class Vector4
{
public:
	Vector4();
	Vector4(const Vector4& vector);
	Vector4(float pmX, float pmY, float pmZ, float pmW);
	~Vector4();

	void zero();
	Vector4 lerp(const Vector4& vector, float t) const;

	float length() const;
	void normalize();

	float dotProduct(const Vector4& vector) const;

	Vector4& operator = (const Vector4& vector);

	Vector4 operator + (const Vector4& vector) const;
	Vector4 operator - (const Vector4& vector) const;

	Vector4& operator += (const Vector4& vector);
	Vector4& operator -= (const Vector4& vector);

	Vector4 operator * (const Matrix4x4& matrix) const;
	Vector4& operator *= (const Matrix4x4& matrix);

	Vector4 operator * (float scalar) const;
	Vector4 operator / (float scalar) const;

	Vector4 operator + (float scalar) const;
	Vector4 operator - (float scalar) const;

	Vector4& operator *= (float scalar);
	Vector4& operator /= (float scalar);

	Vector4& operator += (float scalar);
	Vector4& operator -= (float scalar);

	union
	{
		struct
		{
			float x, y, z, w;
		};

		float components[4];
	};
};

inline void Vector4::zero()
{
	x = y = z = 0.0f;
}

inline Vector4 Vector4::lerp(const Vector4& vector, float t) const
{
	return Vector4
		((vector.x - x) * t + x,
		(vector.y - y) * t + y,
		(vector.z - z) * t + z,
		(vector.w - w) * t + w);
}

inline float Vector4::length() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

inline void Vector4::normalize()
{
	float lvLength = length();
	if (lvLength == 0.0f) return;

	x /= lvLength;
	y /= lvLength;
	z /= lvLength;
	w /= lvLength;
};

inline float Vector4::dotProduct(const Vector4& vector) const
{
	return x * vector.x + y * vector.y + z * vector.z + w * vector.w;
}

inline Vector4& Vector4::operator = (const Vector4& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
	w = vector.w;

	return *this;
}

inline Vector4 Vector4::operator + (const Vector4& vector) const
{
	return Vector4(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
}

inline Vector4 Vector4::operator - (const Vector4& vector) const
{
	return Vector4(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
}

inline Vector4& Vector4::operator += (const Vector4& vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;
	w += vector.w;

	return *this;
}

inline Vector4& Vector4::operator -= (const Vector4& vector)
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;
	w -= vector.w;

	return *this;
}

inline Vector4 Vector4::operator * (const Matrix4x4& matrix) const
{
	return Vector4
		(x * matrix.m11 + y * matrix.m21 + z * matrix.m31 + w * matrix.m41,
		x * matrix.m12 + y * matrix.m22 + z * matrix.m32 + w * matrix.m42,
		x * matrix.m13 + y * matrix.m23 + z * matrix.m33 + w * matrix.m43,
		x * matrix.m14 + y * matrix.m24 + z * matrix.m34 + w * matrix.m44);
}

inline Vector4& Vector4::operator *= (const Matrix4x4& matrix)
{
	float lvX = x;
	float lvY = y;
	float lvZ = z;
	float lvW = w;

	x = lvX * matrix.m11 + lvY * matrix.m21 + lvZ * matrix.m31 + lvW * matrix.m41;
	y = lvX * matrix.m12 + lvY * matrix.m22 + lvZ * matrix.m32 + lvW * matrix.m42;
	z = lvX * matrix.m13 + lvY * matrix.m23 + lvZ * matrix.m33 + lvW * matrix.m43;
	w = lvX * matrix.m14 + lvY * matrix.m24 + lvZ * matrix.m34 + lvW * matrix.m44;

	return *this;
}

inline Vector4 Vector4::operator * (float scalar) const
{
	return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

inline Vector4 Vector4::operator / (float scalar) const
{
	return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

inline Vector4 Vector4::operator + (float scalar) const
{
	return Vector4(x + scalar, y + scalar, z + scalar, w + scalar);
}

inline Vector4 Vector4::operator - (float scalar) const
{
	return Vector4(x - scalar, y - scalar, z - scalar, w - scalar);
}

inline Vector4& Vector4::operator *= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

inline Vector4& Vector4::operator /= (float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;

	return *this;
}

inline Vector4& Vector4::operator += (float scalar)
{
	x += scalar;
	y += scalar;
	z += scalar;
	w += scalar;

	return *this;
}

inline Vector4& Vector4::operator -= (float scalar)
{
	x -= scalar;
	y -= scalar;
	z -= scalar;
	w -= scalar;

	return *this;
}
