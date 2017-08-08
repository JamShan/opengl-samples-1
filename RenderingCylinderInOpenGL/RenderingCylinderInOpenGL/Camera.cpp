#include "Camera.h"

Camera::Camera()
{
	orientation.identity();
	position.y += 2.0f;
}

SpVector3 Camera::direction() const
{
	return SpVector3(0.0f, 0.0f, 1.0f)
		.transformByQuaternion(orientation);
}

void Camera::yaw(float angle)
{
	orientation = SpQuaternion(
		SpVector3(0.0f, 1.0f, 0.0f),
		degreesToRadians(angle)) * orientation;
}

void Camera::pitch(float angle)
{
	orientation *= SpQuaternion(
		SpVector3(1.0f, 0.0f, 0.0f),
		degreesToRadians(angle));
}

void Camera::roll(float angle)
{
	orientation *= SpQuaternion(
		SpVector3(0.0f, 0.0f, 1.0f),
		degreesToRadians(angle));
}

void Camera::forward(float distance)
{
	position += direction() * distance;
}

void Camera::backward(float distance)
{
	position -= direction() * distance;
}

void Camera::left(float distance)
{
	position -= SpVector3(0.0f, 1.0f, 0.0f).cross(direction()) * distance;
}

void Camera::right(float distance)
{
	position += SpVector3(0.0f, 1.0f, 0.0f).cross(direction()) * distance;
}

SpMatrix4x4 Camera::getViewMatrix() const
{
	SpVector3 zAxis = SpVector3(0.0f, 0.0f, 1.0f).transformByQuaternion(orientation);
	SpVector3 xAxis = SpVector3(1.0f, 0.0f, 0.0f).transformByQuaternion(orientation);
	SpVector3 yAxis = SpVector3(0.0f, 1.0f, 0.0f).transformByQuaternion(orientation);

	return SpMatrix4x4(
		xAxis.x, yAxis.x, zAxis.x, 0.0f,
		xAxis.y, yAxis.y, zAxis.y, 0.0f,
		xAxis.z, yAxis.z, zAxis.z, 0.0f,
		-xAxis.dot(position),
		-yAxis.dot(position),
		-zAxis.dot(position),
		1.0f);
}

float Camera::degreesToRadians(float radians) const
{
	const float Pi = 3.14159265359f;

	return radians * (Pi / 180.0f);
}
