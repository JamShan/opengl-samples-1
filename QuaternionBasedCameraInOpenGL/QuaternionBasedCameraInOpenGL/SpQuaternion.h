
#pragma once
#include<cmath>

class SpVector3;

class SpQuaternion
{
public:
	SpQuaternion();
	SpQuaternion(const SpQuaternion& quaternion);
	SpQuaternion(const SpVector3& vector);
	SpQuaternion(const SpVector3& axis, float angle);
	SpQuaternion(float pmW, float pmX, float pmY, float pmZ);
	~SpQuaternion();

	SpQuaternion& fromAxisAngle(const SpVector3& axis, float angle);
	SpQuaternion& identity();
	SpQuaternion& zero();

	SpQuaternion& conjugate();
	SpQuaternion getConjugated() const;

	float length() const;
	SpQuaternion& normalize();

	SpQuaternion slerp(const SpQuaternion& quaternion, float t) const;
	float dot(const SpQuaternion& quaternion) const;

	SpQuaternion& operator = (const SpQuaternion& quaternion);
	SpQuaternion operator * (const SpQuaternion& quaternion) const;

	SpQuaternion operator *= (const SpQuaternion& quaternion);

	float w, x, y, z;
};

inline SpQuaternion& SpQuaternion::identity()
{
	w = 1.0f;
	x = y = z = 0.0f;

	return *this;
}

inline SpQuaternion& SpQuaternion::zero()
{
	w = x = y = z = 0.0f;

	return *this;
}

inline SpQuaternion& SpQuaternion::conjugate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

inline SpQuaternion SpQuaternion::getConjugated() const
{
	return SpQuaternion(w, -x, -y, -z);
}

inline float SpQuaternion::length() const
{
	return sqrtf(w * w + x * x + y * y + z * z);
}

inline SpQuaternion& SpQuaternion::normalize()
{
	float lvLength = length();
	if (lvLength == 0.0f)
		return *this;

	w /= lvLength;
	x /= lvLength;
	y /= lvLength;
	z /= lvLength;

	return *this;
};

inline float SpQuaternion::dot(const SpQuaternion& quaternion) const
{
	return
		w * quaternion.w +
		x * quaternion.x +
		y * quaternion.y +
		z * quaternion.z;
}

inline SpQuaternion& SpQuaternion::operator = (const SpQuaternion& quaternion)
{
	w = quaternion.w;
	x = quaternion.x;
	y = quaternion.y;
	z = quaternion.z;

	return *this;
}

inline SpQuaternion SpQuaternion::operator * (const SpQuaternion& quaternion) const
{
	return SpQuaternion
		(w * quaternion.w - x * quaternion.x - y * quaternion.y - z * quaternion.z,
		w * quaternion.x + x * quaternion.w + y * quaternion.z - z * quaternion.y,
		w * quaternion.y + y * quaternion.w + z * quaternion.x - x * quaternion.z,
		w * quaternion.z + z * quaternion.w + x * quaternion.y - y * quaternion.x);
}

inline SpQuaternion SpQuaternion::operator *= (const SpQuaternion& quaternion)
{
	float lvW = w;
	float lvX = x;
	float lvY = y;
	float lvZ = z;

	w = lvW * quaternion.w - lvX * quaternion.x - lvY * quaternion.y - lvZ * quaternion.z;
	x = lvW * quaternion.x + lvX * quaternion.w + lvY * quaternion.z - lvZ * quaternion.y;
	y = lvW * quaternion.y + lvY * quaternion.w + lvZ * quaternion.x - lvX * quaternion.z;
	z = lvW * quaternion.z + lvZ * quaternion.w + lvX * quaternion.y - lvY * quaternion.x;

	return *this;
}