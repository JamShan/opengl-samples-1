#pragma once

#include "SpVector3.h"
#include "SpQuaternion.h"

const float PI = 3.14159265358979f;
const float PI_2 = PI * 2.0f;

class BoundingCylinder
{
	float volume() const;
	float diameter() const;

	float baseArea() const;
	float lateralArea() const;

	SpVector3 center() const;
	SpVector3 axis() const;
	SpVector3 axialDirection() const;

	SpVector3 position;
	SpQuaternion orientation;

	float radius;
	float height;
};

inline float BoundingCylinder::volume() const
{
	return PI * radius * radius * height;
}

inline float BoundingCylinder::diameter() const
{
	return radius * 2.0f;
}

inline float BoundingCylinder::baseArea() const
{
	return PI * radius * radius;
}

inline float BoundingCylinder::lateralArea() const
{
	return PI_2 * radius * height;
}

inline SpVector3 BoundingCylinder::center() const
{
	return axis() * 0.5f;
}

inline SpVector3 BoundingCylinder::axis() const
{
	return axialDirection() * height;
}

inline SpVector3 BoundingCylinder::axialDirection() const
{
	return SpVector3(0.0f, 1.0f, 0.0f).transformByQuaternion(orientation);
}