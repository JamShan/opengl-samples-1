
#include "SpQuaternion.h"
#include "SpVector3.h"
#include <cmath>

SpQuaternion::SpQuaternion() :
	w(0.0f), x(0.0f), y(0.0f), z(0.0f) {}

SpQuaternion::SpQuaternion(const SpQuaternion& quaternion) :
	w(quaternion.w), x(quaternion.x), y(quaternion.y), z(quaternion.z) {}

SpQuaternion::SpQuaternion(const SpVector3& vector) :
	w(0.0f), x(vector.x), y(vector.y), z(vector.z) {}

SpQuaternion::SpQuaternion(const SpVector3& axis, float angle)
{
	fromAxisAngle(axis, angle);
}

SpQuaternion::SpQuaternion(float pmW, float pmX, float pmY, float pmZ) :
	w(pmW), x(pmX), y(pmY), z(pmZ) {}

SpQuaternion::~SpQuaternion() {}

SpQuaternion& SpQuaternion::fromAxisAngle(const SpVector3& axis, float angle)
{
	float halfAngle = angle / 2.0f;
	float halfAngleSine = sinf(halfAngle);
	float halfAngleCosine = cosf(halfAngle);

	w = halfAngleCosine;
	x = axis.x * halfAngleSine;
	y = axis.y * halfAngleSine;
	z = axis.z * halfAngleSine;

	return *this;
}

SpQuaternion SpQuaternion::slerp(const SpQuaternion& quaternion, float t) const
{
	float lvW = w;
	float lvX = x;
	float lvY = y;
	float lvZ = z;

	float lvDotProduct =
		lvW * quaternion.w +
		lvX * quaternion.x +
		lvY * quaternion.y +
		lvZ * quaternion.z;

	float lvScalar0 = 0.0f;
	float lvScalar1 = 0.0f;

	if (lvDotProduct < 0.0f)
	{
		lvW = -lvW;
		lvX = -lvX;
		lvY = -lvY;
		lvZ = -lvZ;
	}

	if (lvDotProduct > 0.9999f)
	{
		lvScalar0 = 1.0f - t;
		lvScalar1 = t;
	}
	else
	{
		float lvSineOmega = sqrtf(1.0f - lvDotProduct * lvDotProduct);
		float lvOmega = atan2(lvSineOmega, lvDotProduct);
		float lvOneDividedBySine = 1.0f / lvSineOmega;

		lvScalar0 = sinf((1.0f - t) * lvOmega) * lvOneDividedBySine;
		lvScalar1 = sinf(t * lvOmega) * lvOneDividedBySine;
	}

	return SpQuaternion
		(lvScalar0 * w + lvScalar1 * lvW,
		lvScalar0 * x + lvScalar1 * lvX,
		lvScalar0 * y + lvScalar1 * lvY,
		lvScalar0 * z + lvScalar1 * lvZ);
}

