#pragma once

#include <gl\glew.h>
#include <GL\wglew.h>
#include <gl\gl.h>

struct IWICBitmap;

class FontRenderer
{
public:
	struct VertexPosition
	{
		GLfloat x, y;
	};

	struct VertexTexturePosition
	{
		GLfloat u, v;
	};

	struct Triangle
	{
		GLuint v1, v2, v3;
	};

	struct Quadrilateral
	{
		Triangle t1, t2;
	};

	struct Glyph
	{
		char32_t codePoint;
		float x;
		float y;
		float width;
		float height;
		float leftSideBearing;
		float rightSideBearing;
		float topSideBearing;
		float bottomSideBearing;
		float advanceWidth;
		float advanceHeight;
	};

	struct KerningPair
	{
		unsigned int first;
		unsigned int second;
		float kernAmount;
	};

	FontRenderer();
	~FontRenderer();

	void loadGlyphs();
	void render(float screenWidth, float screenHeight);

	float getKernAmount(unsigned int first, unsigned int second)
	{
		for (size_t i = 0; i < 88; ++i)
		{
			KerningPair& kerningPair = kerningPairs[i];
			
			if (kerningPair.first == first && kerningPair.second == second)
				return kerningPair.kernAmount;
		}

		return 0.0f;
	}

	Glyph glyphs[94];
	KerningPair kerningPairs[88];

	GLuint vertexArray;
	GLuint vertexPositionBuffer;
	GLuint vertexTexturePositionBuffer;
	GLuint indexBuffer;
	GLuint texture;
	IWICBitmap* wicBitmap;
};