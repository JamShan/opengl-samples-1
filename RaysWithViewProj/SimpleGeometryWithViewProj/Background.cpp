#include "Background.h"
#include "Vector2.h"
#include "Bitmap.h"

struct Vertex
{
	GLfloat x, y;
	GLfloat u, v;
};

const GLchar* BackgroundVS = "\
	#version 450 core\n\
	layout(location = 0) in vec2 in_Position;\n\
	layout(location = 1) in vec2 in_UV;\n\
	out vec2 UV;\n\
	void main() {\n\
		gl_Position = vec4(in_Position, 0.0, 1.0);\n\
		UV = in_UV;\n\
	}";

const GLchar* BackgroundFS = "\
	#version 450 core\n\
	uniform sampler2D tex;\n\
	in vec2 UV;\n\
	out vec4 colorOut;\n\
	void main() {\n\
		colorOut = texture(tex, UV);\n\
	}";

Background::Background()
{
	program = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &BackgroundVS, NULL);
	glShaderSource(fragmentShader, 1, &BackgroundFS, NULL);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	Bitmap bitmap(L"background.jpg", true);

	float halfWidth = 1.0f;
	float halfheight = 1.0f;

	Vertex vertices[4] =
	{
		{ -halfWidth, -halfheight, 0.0f, 0.0f},
		{ -halfWidth, halfheight, 0.0f, 1.0f},
		{ halfWidth, halfheight, 1.0f, 1.0f},
		{ halfWidth, -halfheight, 1.0f, 0.0f}
	};

	GLuint indices[6] = { 0,1,2,0,2,3 };

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if (bitmap.getPixelFormat() == GL_BGR)
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, bitmap.getWidth(), bitmap.getHeight());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap.getWidth(), bitmap.getHeight(), GL_BGR, GL_UNSIGNED_BYTE, bitmap.getPixels());
	}
	else if(bitmap.getPixelFormat() == GL_BGRA)
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, bitmap.getWidth(), bitmap.getHeight());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bitmap.getWidth(), bitmap.getHeight(), GL_BGR, GL_UNSIGNED_BYTE, bitmap.getPixels());
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0);

	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (GLvoid*)(sizeof(GLfloat) * 2));

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Background::~Background()
{
	glDeleteTextures(1, &texture);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &vertexArray);
}

void Background::render()
{
	glDisable(GL_DEPTH_TEST);
	glUseProgram(program);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vertexArray);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}