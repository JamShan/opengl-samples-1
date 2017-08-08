#pragma once

#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>

class Background
{
public:
	Background();
	~Background();

	void render();
private:
	GLuint program;
	GLuint texture;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint indexBuffer;
};