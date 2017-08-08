#pragma once

#include <cmath>

class Matrix4x4;

class Vector3
{
public:
	Vector3();
	Vector3(const Vector3& vector);
	Vector3(float pmX, float pmY, float pmZ);
	~Vector3();

	void zero();
	Vector3 lerp(const Vector3& vector, float t) const;

	float length() const;
	Vector3& normalize();

	float dotProduct(const Vector3& vector) const;
	Vector3 crossProduct(const Vector3& vector) const;

	Vector3& operator = (const Vector3& vector);

	Vector3 operator + (const Vector3& vector) const;
	Vector3 operator - (const Vector3& vector) const;

	Vector3& operator += (const Vector3& vector);
	Vector3& operator -= (const Vector3& vector);

	Vector3 operator * (const Matrix4x4& matrix) const;
	Vector3& operator *= (const Matrix4x4& matrix);

	Vector3 operator * (float scalar) const;
	Vector3 operator / (float scalar) const;

	Vector3 operator + (float scalar) const;
	Vector3 operator - (float scalar) const;

	Vector3& operator *= (float scalar);
	Vector3& operator /= (float scalar);

	Vector3& operator += (float scalar);
	Vector3& operator -= (float scalar);

	union
	{
		struct
		{
			float x, y, z;
		};

		float components[3];
	};
};

inline void Vector3::zero()
{
	x = y = z = 0.0f;
}

inline Vector3 Vector3::lerp(const Vector3& vector, float t) const
{
	return Vector3
		((vector.x - x) * t + x,
		(vector.y - y) * t + y,
		(vector.z - z) * t + z);
}

inline float Vector3::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

inline Vector3& Vector3::normalize()
{
	float lvLength = length();

	if (lvLength != 0.0f)
	{
		x /= lvLength;
		y /= lvLength;
		z /= lvLength;
	}

	return *this;
};

inline float Vector3::dotProduct(const Vector3& vector) const
{
	return x * vector.x + y * vector.y + z * vector.z;
}

inline Vector3 Vector3::crossProduct(const Vector3& vector) const
{
	return Vector3
		(y * vector.z - z * vector.y,
		z * vector.x - x * vector.z,
		x * vector.y - y * vector.x);
}

inline Vector3& Vector3::operator = (const Vector3& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;

	return *this;
}

inline Vector3 Vector3::operator + (const Vector3& vector) const
{
	return Vector3(x + vector.x, y + vector.y, z + vector.z);
}

inline Vector3 Vector3::operator - (const Vector3& vector) const
{
	return Vector3(x - vector.x, y - vector.y, z - vector.z);
}

inline Vector3& Vector3::operator += (const Vector3& vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;

	return *this;
}

inline Vector3& Vector3::operator -= (const Vector3& vector)
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;

	return *this;
}

inline Vector3 Vector3::operator * (float scalar) const
{
	return Vector3(x * scalar, y * scalar, z * scalar);
}

inline Vector3 Vector3::operator / (float scalar) const
{
	return Vector3(x / scalar, y / scalar, z / scalar);
}

inline Vector3 Vector3::operator + (float scalar) const
{
	return Vector3(x + scalar, y + scalar, z + scalar);
}

inline Vector3 Vector3::operator - (float scalar) const
{
	return Vector3(x - scalar, y - scalar, z - scalar);
}

inline Vector3& Vector3::operator *= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

inline Vector3& Vector3::operator /= (float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;

	return *this;
}

inline Vector3& Vector3::operator += (float scalar)
{
	x += scalar;
	y += scalar;
	z += scalar;

	return *this;
}

inline Vector3& Vector3::operator -= (float scalar)
{
	x -= scalar;
	y -= scalar;
	z -= scalar;

	return *this;
}
