#include "Vector4.h"

Vector4::Vector4() :
	x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Vector4::Vector4(const Vector4& vector) :
	x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}

Vector4::Vector4(float pmX, float pmY, float pmZ, float pmW) :
	x(pmX), y(pmY), z(pmZ), w(pmW) {}

Vector4::~Vector4() {};