
#pragma once

#include <cmath>

/*
	Three-Dimensional Vector For Single-Precision Floating-Point Format

	Naming Convention:

	pm - Parameter
	lv - Local Variable
*/

class SpVector3
{
	public:

		SpVector3();
		SpVector3(SpVector3& vector);
		SpVector3(float pmX, float pmY, float pmZ);
		~SpVector3();

		void zero();
		SpVector3 lerp(const SpVector3& vector, float t) const;

		float length() const;
		void normalize();

		float dotProduct(SpVector3& vector) const;
		SpVector3 crossProduct(SpVector3& vector) const;

		SpVector3& operator = (const SpVector3& vector);

		SpVector3 operator + (const SpVector3& vector) const;
		SpVector3 operator - (const SpVector3& vector) const;

		SpVector3& operator += (const SpVector3& vector);
		SpVector3& operator -= (const SpVector3& vector);

		SpVector3 operator * (float scalar) const;
		SpVector3 operator / (float scalar) const;

		SpVector3 operator + (float scalar) const;
		SpVector3 operator - (float scalar) const;

		SpVector3& operator *= (float scalar);
		SpVector3& operator /= (float scalar);

		SpVector3& operator += (float scalar);
		SpVector3& operator -= (float scalar);

		float x, y, z;
};

inline void SpVector3::zero()
{
	x = y = z = 0.0f;
}

inline SpVector3 SpVector3::lerp(const SpVector3& vector, float t) const
{
	return SpVector3
		((vector.x - x) * t + x,
		(vector.y - y) * t + y,
		(vector.z - z) * t + z);
}

inline float SpVector3::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

inline void SpVector3::normalize()
{
	float lvLength = length();
	if (lvLength == 0.0f) return;

	x /= lvLength;
	y /= lvLength;
	z /= lvLength;
};

inline float SpVector3::dotProduct(SpVector3& vector) const
{
	return x * vector.x + y * vector.y + z * vector.z;
}

inline SpVector3 SpVector3::crossProduct(SpVector3& vector) const
{
	return SpVector3
		(y * vector.z - z * vector.y,
		z * vector.x - x * vector.z,
		x * vector.y - y * vector.x);
}

inline SpVector3& SpVector3::operator = (const SpVector3& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;

	return *this;
}

inline SpVector3 SpVector3::operator + (const SpVector3& vector) const
{
	return SpVector3(x + vector.x, y + vector.y, z + vector.z);
}

inline SpVector3 SpVector3::operator - (const SpVector3& vector) const
{
	return SpVector3(x - vector.x, y - vector.y, z - vector.z);
}

inline SpVector3& SpVector3::operator += (const SpVector3& vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;

	return *this;
}

inline SpVector3& SpVector3::operator -= (const SpVector3& vector)
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;

	return *this;
}

inline SpVector3 SpVector3::operator * (float scalar) const
{
	return SpVector3(x * scalar, y * scalar, z * scalar);
}

inline SpVector3 SpVector3::operator / (float scalar) const
{
	return SpVector3(x / scalar, y / scalar, z / scalar);
}

inline SpVector3 SpVector3::operator + (float scalar) const
{
	return SpVector3(x + scalar, y + scalar, z + scalar);
}

inline SpVector3 SpVector3::operator - (float scalar) const
{
	return SpVector3(x - scalar, y - scalar, z - scalar);
}

inline SpVector3& SpVector3::operator *= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

inline SpVector3& SpVector3::operator /= (float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;

	return *this;
}

inline SpVector3& SpVector3::operator += (float scalar)
{
	x += scalar;
	y += scalar;
	z += scalar;

	return *this;
}

inline SpVector3& SpVector3::operator -= (float scalar)
{
	x -= scalar;
	y -= scalar;
	z -= scalar;

	return *this;
}
