#pragma once

#include <string>
#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>

class Bitmap
{
public:
	Bitmap(const std::wstring& path, bool flipVertically);
	~Bitmap();

	GLubyte* getPixels() const;
	GLenum getPixelFormat() const;

	GLuint getWidth() const;
	GLuint getHeight() const;
private:
	GLubyte* pixels;
	GLenum pixelFormat;

	GLuint width;
	GLuint height;
};

inline GLuint Bitmap::getWidth() const
{
	return width;
}

inline GLuint Bitmap::getHeight() const
{
	return height;
}

inline GLubyte* Bitmap::getPixels() const
{
	return pixels;
}

inline GLenum Bitmap::getPixelFormat() const
{
	return pixelFormat;
}