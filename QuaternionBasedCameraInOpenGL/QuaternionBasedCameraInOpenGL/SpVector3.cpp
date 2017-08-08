
#include "SpVector3.h"
#include "SpQuaternion.h"

SpVector3::SpVector3() :
	x(0.0f), y(0.0f), z(0.0f) {}

SpVector3::SpVector3(const SpVector3& vector) :
	x(vector.x), y(vector.y), z(vector.z) {}

SpVector3::SpVector3(float pmX, float pmY, float pmZ) :
	x(pmX), y(pmY), z(pmZ) {}

SpVector3::~SpVector3() {};

SpVector3& SpVector3::transformByQuaternion(const SpQuaternion& quaternion)
{
	SpQuaternion result = quaternion * SpQuaternion(*this) * quaternion.getConjugated();
	x = result.x;
	y = result.y;
	z = result.z;

	return *this;
}