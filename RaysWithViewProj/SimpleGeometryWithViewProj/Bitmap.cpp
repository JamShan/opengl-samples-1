#include "Bitmap.h"
#include <wincodec.h>

Bitmap::Bitmap(const std::wstring& path, bool flipVertically)
{
	GUID containerFormat;

	if (path.find(L".bmp") != std::wstring::npos)
		containerFormat = GUID_ContainerFormatBmp;

	else if (path.find(L".png") != std::wstring::npos)
		containerFormat = GUID_ContainerFormatPng;

	else if (path.find(L".tif") != std::wstring::npos)
		containerFormat = GUID_ContainerFormatTiff;

	else if (path.find(L".tiff") != std::wstring::npos)
		containerFormat = GUID_ContainerFormatTiff;

	else if (path.find(L".jpg") != std::wstring::npos)
		containerFormat = GUID_ContainerFormatJpeg;

	else if (path.find(L".dds") != std::wstring::npos)
		containerFormat = GUID_ContainerFormatDds;

	else
		throw std::invalid_argument("Hey, listen, something is wrong with the path you have provided, please make sure it's ok");

	IWICImagingFactory* factory;
	CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));

	IWICStream* stream;
	factory->CreateStream(&stream);
	stream->InitializeFromFilename(path.c_str(), GENERIC_READ);

	IWICBitmapDecoder* decoder;
	factory->CreateDecoder(containerFormat, nullptr, &decoder);
	decoder->Initialize(stream, WICDecodeMetadataCacheOnDemand);

	IWICBitmapFrameDecode* decode;
	decoder->GetFrame(0, &decode);

	IWICBitmap* bitmap;

	if (flipVertically)
	{
		IWICBitmapFlipRotator* flipRotator;
		factory->CreateBitmapFlipRotator(&flipRotator);

		flipRotator->Initialize(decode, WICBitmapTransformFlipVertical);
		factory->CreateBitmapFromSource(flipRotator, WICBitmapNoCache, &bitmap);
		flipRotator->Release();
	}
	else
		factory->CreateBitmapFromSource(decode, WICBitmapNoCache, &bitmap);

	bitmap->GetSize(&width, &height);

	WICPixelFormatGUID lvPixelFormat;
	bitmap->GetPixelFormat(&lvPixelFormat);

	if (lvPixelFormat == GUID_WICPixelFormat24bppRGB)
		pixelFormat = GL_RGB;

	else if (lvPixelFormat == GUID_WICPixelFormat24bppBGR)
		pixelFormat = GL_BGR;

	else if (lvPixelFormat == GUID_WICPixelFormat32bppRGBA)
		pixelFormat = GL_RGBA;

	else if (lvPixelFormat == GUID_WICPixelFormat32bppBGRA)
		pixelFormat = GL_BGRA;

	WICRect rect = {0};
	rect.Width = static_cast<INT> (width);
	rect.Height = static_cast<INT> (height);

	IWICBitmapLock* lock;
	bitmap->Lock(&rect, WICBitmapLockWrite, &lock);

	UINT stride;
	lock->GetStride(&stride);

	UINT bufferSize;
	WICInProcPointer ptr;

	lock->GetDataPointer(&bufferSize, &ptr);
	pixels = new GLubyte[bufferSize];
	memcpy_s(pixels, bufferSize, ptr, bufferSize);

	lock->Release();
	bitmap->Release();
	decode->Release();
	decoder->Release();
	stream->Release();
	factory->Release();
}

Bitmap::~Bitmap()
{
	delete pixels;
}