#pragma once
#include <cmath>

class Vector2
{
public:
	Vector2();
	Vector2(const Vector2& vector);
	Vector2(float pmx, float pmy);
	~Vector2();

	void zero();
	Vector2 lerp(const Vector2& vector, float t) const;

	float length() const;
	Vector2& normalize();

	float dotProduct(Vector2& vector) const;
	float crossProduct(Vector2& vector) const;

	Vector2& operator = (const Vector2& vector);

	Vector2 operator + (const Vector2& vector) const;
	Vector2 operator - (const Vector2& vector) const;

	Vector2& operator += (const Vector2& vector);
	Vector2& operator -= (const Vector2& vector);

	Vector2 operator * (float scalar) const;
	Vector2 operator / (float scalar) const;

	Vector2 operator + (float scalar) const;
	Vector2 operator - (float scalar) const;

	Vector2& operator *= (float scalar);
	Vector2& operator /= (float scalar);

	Vector2& operator += (float scalar);
	Vector2& operator -= (float scalar);

	union
	{
		struct
		{
			float x, y;
		};

		float components[2];
	};
};

inline void Vector2::zero()
{
	x = y = 0.0f;
}

inline Vector2 Vector2::lerp(const Vector2& vector, float t) const
{
	return Vector2((vector.x - x) * t + x, (vector.y - y) * t + y);
}

inline float Vector2::length() const
{
	return sqrtf(x * x + y * y);
}

inline Vector2& Vector2::normalize()
{
	float lvLength = length();

	if (lvLength != 0.0f) {

		x /= lvLength;
		y /= lvLength;
	}

	return *this;
}

inline float Vector2::dotProduct(Vector2& vector) const
{
	return x * vector.x + y * vector.y;
}

inline float Vector2::crossProduct(Vector2& vector) const
{
	return x * vector.y - y * vector.x;
}

inline Vector2& Vector2::operator = (const Vector2& vector)
{
	x = vector.x;
	y = vector.y;

	return *this;
};

inline Vector2 Vector2::operator + (const Vector2& vector) const
{
	return Vector2(x + vector.x, y + vector.y);
}

inline Vector2 Vector2::operator - (const Vector2& vector) const
{
	return Vector2(x - vector.x, y - vector.y);
}

inline Vector2& Vector2::operator += (const Vector2& vector)
{
	x += vector.x;
	y += vector.y;

	return *this;
}

inline Vector2& Vector2::operator -= (const Vector2& vector)
{
	x -= vector.x;
	y -= vector.y;

	return *this;
}

inline Vector2 Vector2::operator * (float scalar) const
{
	return Vector2(x * scalar, y * scalar);
}

inline Vector2 Vector2::operator / (float scalar) const
{
	return Vector2(x / scalar, y / scalar);
}

inline Vector2 Vector2::operator + (float scalar) const
{
	return Vector2(x + scalar, y + scalar);
}

inline Vector2 Vector2::operator - (float scalar) const
{
	return Vector2(x - scalar, y - scalar);
}

inline Vector2& Vector2::operator *= (float scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

inline Vector2& Vector2::operator /= (float scalar)
{
	x /= scalar;
	y /= scalar;

	return *this;
}

inline Vector2& Vector2::operator += (float scalar)
{
	x += scalar;
	y += scalar;

	return *this;
}

inline Vector2& Vector2::operator -= (float scalar)
{
	x -= scalar;
	y -= scalar;

	return *this;
}