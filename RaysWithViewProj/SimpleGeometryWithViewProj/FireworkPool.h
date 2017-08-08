#pragma once

#include <list>
#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Firework.h"
#include <ctime>

class Firework;

class FireworkPool
{
public:
	FireworkPool(float screenWidth, float screenHeight);
	~FireworkPool();

	void launch(const Vector2& position);
	void updateProjectionMatrix(float screenWidth, float screenHeight);

	void update();
	void render();
private:
	std::list<Firework*> availableFireworks;
	std::list<Firework*> playingFireworks;

	GLuint transformFeedback;
	GLuint updateProgram;
	GLuint renderProgram;

	Matrix4x4 view;
	Matrix4x4 projection;
};

inline void FireworkPool::launch(const Vector2& position)
{
	if (availableFireworks.size())
	{
		Firework& firework = *availableFireworks.front();
		Matrix4x4 viewProjectionInverse = (view * projection).invert();
		Vector3 position3D(position.x, position.y, 0.0f);

		position3D *= viewProjectionInverse;
		firework.setPosition(position3D);

		firework.setTiming(false);
		firework.setUpdateTime(0);
		firework.setElapsedTime(0);

		playingFireworks.push_back(&firework);
		availableFireworks.pop_front();
	}
}

inline void FireworkPool::updateProjectionMatrix(float screenWidth, float screenHeight)
{
	projection.perspectiveProjectionFovLhOgl(
		45.0f * (3.14159265359f / 180.0f),
		screenWidth / screenHeight,
		1.0f,
		100.0f);

	GLint projectionMatrixLocation = glGetUniformLocation(updateProgram, "projection");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection.m11);
}