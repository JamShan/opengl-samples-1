#pragma once

#include "SpMatrix4x4.h"
#include "SpQuaternion.h"
#include "SpVector3.h"

class Camera
{
public:
	Camera();
	SpVector3 direction() const;

	void yaw(float angle);
	void pitch(float angle);
	void roll(float angle);

	void forward(float distance);
	void backward(float distance);

	void left(float distance);
	void right(float distance);

	SpMatrix4x4 getViewMatrix() const;
	float degreesToRadians(float radians) const;

	SpQuaternion orientation;
	SpVector3 position;
};