#include "FontRenderer.h"
#include <Windows.h>
#include <wincodec.h>
#include "Vector2.h"

const size_t numGlyphs = 94;
const size_t numKerningPairs = 88;

FontRenderer::FontRenderer()
{
	Quadrilateral* quads = new Quadrilateral[numGlyphs];

	size_t i = 0;
	size_t v = 0;

	while (i < numGlyphs)
	{
		quads[i].t1.v1 = v;
		quads[i].t1.v2 = v + 1;
		quads[i].t1.v3 = v + 2;

		quads[i].t2.v1 = v + 2;
		quads[i].t2.v2 = v + 3;
		quads[i].t2.v3 = v;

		++i;
		v += 4;
	}

	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexPositionBuffer);
	glGenBuffers(1, &vertexTexturePositionBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindVertexArray(vertexArray);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPosition) * 4 * numGlyphs, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(VertexPosition), 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexTexturePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexturePosition) * 4 * numGlyphs, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexTexturePosition), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Quadrilateral) * numGlyphs, quads, GL_STATIC_DRAW);

	delete[]quads;
}

FontRenderer::~FontRenderer() {}

void FontRenderer::loadGlyphs()
{
	IWICImagingFactory* wicFactory;
	IWICBitmapDecoder* decoder;
	IWICBitmapFrameDecode* bitmapFrameDecode;
	IWICStream* stream;

	HANDLE file = CreateFile(L"bitmapFontData.bin", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD readBytes;
	ReadFile(file, glyphs, sizeof(Glyph) * numGlyphs, &readBytes, 0);
	ReadFile(file, kerningPairs, sizeof(KerningPair) * numKerningPairs, &readBytes, 0);
	CloseHandle(file);

	CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
	wicFactory->CreateStream(&stream);
	stream->InitializeFromFilename(L"Bitmap.png", GENERIC_READ);

	GUID containerFormat = GUID_ContainerFormatPng;
	wicFactory->CreateDecoder(containerFormat, nullptr, &decoder);
	decoder->Initialize(stream, WICDecodeMetadataCacheOnDemand);
	decoder->GetFrame(0, &bitmapFrameDecode);

	IWICBitmapFlipRotator* flipRotator;
	wicFactory->CreateBitmapFlipRotator(&flipRotator);
	flipRotator->Initialize(bitmapFrameDecode, WICBitmapTransformFlipVertical);
	wicFactory->CreateBitmapFromSource(flipRotator, WICBitmapNoCache, &wicBitmap);

	flipRotator->Release();
	bitmapFrameDecode->Release();
	decoder->Release();
	stream->Release();

	IWICBitmapLock* lock;
	WICRect rect;
	WICInProcPointer ptr;

	UINT w, h, s, bs;
	wicBitmap->GetSize(&w, &h);

	rect.X = 0;
	rect.Y = 0;
	rect.Width = w;
	rect.Height = h;

	wicBitmap->Lock(&rect, WICBitmapLockWrite, &lock);
	lock->GetStride(&s);
	lock->GetDataPointer(&bs, &ptr);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, w, h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_BGRA_EXT, GL_UNSIGNED_BYTE, ptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE0);
	lock->Release();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void FontRenderer::render(float screenWidth, float screenHeight)
{
	Vector2 screenCenter(screenWidth * 0.5f, screenHeight * 0.5f);
	VertexPosition vertexPositions[4];
	VertexTexturePosition vertexTexturePositions[4];
	glBindVertexArray(vertexArray);

	float maxHeight = 0.0f;
	float advancex = 0;
	float advancey = 0;
	size_t counter = 0;

	Glyph *avatar[6];
	avatar[0] = &glyphs[32];//A
	avatar[1] = &glyphs[53];//V
	avatar[2] = &glyphs[32];//A

	avatar[3] = &glyphs[51];//T
	avatar[4] = &glyphs[32];//A
	avatar[5] = &glyphs[49];//R

	unsigned int indices[6] = {32,53,32,51,32,49};

	for (size_t i = 0; i < 6; ++i)
	{
		if (counter == 10)
		{
			advancex = 0.0f;
			advancey += maxHeight;
			maxHeight = 0.0f;
			counter = 0;
		}

		Glyph& glyph = *avatar[i];//glyphs[i];
		advancex += glyph.leftSideBearing;

		float xInGLSpace = (advancex - screenCenter.x) / screenCenter.x;
		float yInGLSpace = (advancey - screenCenter.y) / screenCenter.y;
		yInGLSpace *= -1.0f;

		float widthInGLSpace = glyph.width / screenCenter.x;
		float heightInGLSpace = glyph.height / screenCenter.y;

		vertexPositions[0].x = xInGLSpace;
		vertexPositions[0].y = yInGLSpace - heightInGLSpace;

		vertexPositions[1].x = xInGLSpace;
		vertexPositions[1].y = yInGLSpace;

		vertexPositions[2].x = xInGLSpace + widthInGLSpace;
		vertexPositions[2].y = yInGLSpace;

		vertexPositions[3].x = xInGLSpace + widthInGLSpace;
		vertexPositions[3].y = yInGLSpace - heightInGLSpace;

		glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(VertexPosition) * 4 * i, sizeof(VertexPosition) * 4, vertexPositions);

		vertexTexturePositions[0].u = glyph.x / 800.0f;
		vertexTexturePositions[0].v = 1.0f - ((glyph.y + glyph.height) / 600.0f);

		vertexTexturePositions[1].u = glyph.x / 800.0f;
		vertexTexturePositions[1].v = 1.0f - (glyph.y / 600.0f);

		vertexTexturePositions[2].u = (glyph.x + glyph.width) / 800.0f;
		vertexTexturePositions[2].v = 1.0f - (glyph.y / 600.0f);

		vertexTexturePositions[3].u = (glyph.x + glyph.width) / 800.0f;
		vertexTexturePositions[3].v = 1.0f - ((glyph.y + glyph.height) / 600.0f);

		glBindBuffer(GL_ARRAY_BUFFER, vertexTexturePositionBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(VertexTexturePosition) * 4 * i, sizeof(VertexTexturePosition) * 4, vertexTexturePositions);

		//glDrawElements(GL_TRIANGLES, 6 * 1, GL_UNSIGNED_INT, (void*)(i * sizeof(GLuint)*6));
		advancex += glyph.advanceWidth;

		float kernAmount = 0.0f;
		if (i != 5)
			kernAmount = getKernAmount(indices[i], indices[i + 1]);
		advancex += kernAmount;

		float height = glyph.height;

		//if (height > maxHeight)
			//maxHeight = height;

		++counter;
	}

	//glBindVertexArray(vertexArray);
	glDrawElements(GL_TRIANGLES, 6 * numGlyphs, GL_UNSIGNED_INT, 0);
}