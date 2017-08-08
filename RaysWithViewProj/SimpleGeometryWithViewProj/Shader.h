#pragma once

#include <string>
#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>

enum class ShaderType: unsigned char
{
	Vertex, Fragment
};

class Shader
{
public:
	Shader(const std::wstring& path, ShaderType type);
	~Shader();
	GLuint getNativeShader()const;
private:
	GLuint nativeShader;
};

inline GLuint Shader::getNativeShader() const
{
	return nativeShader;
}