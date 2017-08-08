#include "FireworkPool.h"
#include "Shader.h"

FireworkPool::FireworkPool(float screenWidth, float screenHeight)
{
	view.lookAtLh(
		Vector3(0.0f, 0.0f, -1.0f),
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 1.0f, 0.0f));

	projection.perspectiveProjectionFovLhOgl(
		45.0f * (3.14159265359f / 180.0f),
		screenWidth / screenHeight,
		1.0f,
		100.0f);

	for (size_t i = 0; i < 10; ++i)
		availableFireworks.push_back(new Firework());

	updateProgram = glCreateProgram();
	renderProgram = glCreateProgram();

	Shader updateVertexShader(L"Update.vs", ShaderType::Vertex);
	Shader renderVertexShader(L"Render.vs", ShaderType::Vertex);
	Shader renderFragmentShader(L"Render.fs", ShaderType::Fragment);

	glAttachShader(updateProgram, updateVertexShader.getNativeShader());

	glGenTransformFeedbacks(1, &transformFeedback);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback);

	const char* varying_names[] = { "out_Position", "out_PreviousPosition", "out_Direction", "out_Color", "out_Life", "out_Speed", "out_Size" };
	glTransformFeedbackVaryings(updateProgram, 7, varying_names, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(updateProgram);
	glDetachShader(updateProgram, updateVertexShader.getNativeShader());

	glAttachShader(renderProgram, renderVertexShader.getNativeShader());
	glAttachShader(renderProgram, renderFragmentShader.getNativeShader());

	glLinkProgram(renderProgram);
	glDetachShader(renderProgram, renderVertexShader.getNativeShader());
	glDetachShader(renderProgram, renderFragmentShader.getNativeShader());

	glUseProgram(updateProgram);

	GLint viewMatrixLocation = glGetUniformLocation(updateProgram, "view");
	GLint projectionMatrixLocation = glGetUniformLocation(updateProgram, "projection");
	
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view.m11);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection.m11);

	glUseProgram(renderProgram);

	viewMatrixLocation = glGetUniformLocation(renderProgram, "view");
	projectionMatrixLocation = glGetUniformLocation(renderProgram, "projection");

	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view.m11);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection.m11);

	glUseProgram(0);
}

FireworkPool::~FireworkPool()
{
	for (auto a = availableFireworks.begin(); a != availableFireworks.end(); ++a)
		delete *a;

	for (auto p = availableFireworks.begin(); p != availableFireworks.end(); ++p)
		delete *p;

	glDeleteTransformFeedbacks(1, &transformFeedback);
	glDeleteProgram(updateProgram);
	glDeleteProgram(renderProgram);
}

void FireworkPool::update()
{
	glUseProgram(updateProgram);

	GLint emitterPositionLocation = glGetUniformLocation(updateProgram, "emitterPosition");
	GLint deltaTimeLocation = glGetUniformLocation(updateProgram, "deltaTime");

	for (auto i = playingFireworks.begin(); i != playingFireworks.end(); ++i)
	{
		Vector3& position = (*i)->getPosition();
		glUniform3fv(emitterPositionLocation, 1, &position.x);

		unsigned int deltaTime = 0;
		unsigned int currentTime = clock() / CLOCKS_PER_SEC;
		unsigned int elapsedTime = 0;

		if (!(*i)->getTiming())
			(*i)->setTiming(true);
		else
		{
			deltaTime = currentTime - (*i)->getUpdateTime();
			elapsedTime = (*i)->getElapsedTime() + deltaTime;
		}

		if (elapsedTime >= 1)
		{
			(*i)->resetBuffer();
			availableFireworks.push_back(*i);
			playingFireworks.remove(*i);

			return;
		}

		(*i)->setUpdateTime(currentTime);
		(*i)->setElapsedTime(elapsedTime);

		glUniform1i(deltaTimeLocation, deltaTime);
		(*i)->update();
	}

	glUseProgram(0);
}

void FireworkPool::render()
{
	glUseProgram(renderProgram);

	for (auto i = playingFireworks.begin(); i != playingFireworks.end(); ++i)
		(*i)->render();

	glUseProgram(0);
}