#include "Vector3.h"
#include "Matrix4x4.h"

Vector3::Vector3() :
	x(0.0f), y(0.0f), z(0.0f) {}

Vector3::Vector3(const Vector3& vector) :
	x(vector.x), y(vector.y), z(vector.z) {}

Vector3::Vector3(float pmX, float pmY, float pmZ) :
	x(pmX), y(pmY), z(pmZ) {}

Vector3::~Vector3() {};

Vector3 Vector3::operator * (const Matrix4x4& matrix) const
{
	return Vector3
	(x * matrix.m11 + y * matrix.m21 + z * matrix.m31,
		x * matrix.m12 + y * matrix.m22 + z * matrix.m32,
		x * matrix.m13 + y * matrix.m23 + z * matrix.m33);
}

Vector3& Vector3::operator *= (const Matrix4x4& matrix)
{
	float lvX = x;
	float lvY = y;
	float lvZ = z;

	x = lvX * matrix.m11 + lvY * matrix.m21 + lvZ * matrix.m31;
	y = lvX * matrix.m12 + lvY * matrix.m22 + lvZ * matrix.m32;
	z = lvX * matrix.m13 + lvY * matrix.m23 + lvZ * matrix.m33;

	return *this;
}