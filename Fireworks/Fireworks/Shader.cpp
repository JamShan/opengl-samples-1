#include "Shader.h"
#include <fstream>
#include <iostream>

Shader::Shader(const std::wstring& path, ShaderType type)
{
	std::ifstream file(path);
	std::string code;

	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
			code += line + "\n";

		file.close();
	}

	GLenum nativeShaderType;

	switch (type)
	{
	case ShaderType::Vertex:
		nativeShaderType = GL_VERTEX_SHADER;
		break;

	case ShaderType::Fragment:
		nativeShaderType = GL_FRAGMENT_SHADER;
		break;
	}

	nativeShader = glCreateShader(nativeShaderType);

	const char* c_str = code.c_str();
	glShaderSource(nativeShader, 1, &c_str, 0);
	glCompileShader(nativeShader);
}

Shader::~Shader()
{
	glDeleteShader(nativeShader);
}