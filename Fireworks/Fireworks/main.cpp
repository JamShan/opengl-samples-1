#include <windows.h>
#include <windowsx.h>
#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>
#include <vector>
#include "Matrix4x4.h"
#include "Background.h"
#include "FireworkPool.h"
#include "Vector2.h"

#pragma comment(lib, "opengl32.lib")

Background* background = nullptr;
FireworkPool* fireworkPool = nullptr;

int screenWidth = 1024;
int screenHeight = 768;

void ResizeViewport(int width, int height)
{
	glViewport(0, 0, width, height);

	if(fireworkPool)
	fireworkPool->updateProjectionMatrix(
		static_cast<float>(width), static_cast<float>(height));
}

void Render(HWND hWnd)
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	background->render();

	fireworkPool->update();
	fireworkPool->render();

	SwapBuffers(GetDC(hWnd));
}

void Initialize(HWND hWnd)
{
	HDC hDC = GetDC(hWnd);
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 32;
	pixelFormatDescriptor.cDepthBits = 32;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 0;

	int pixelformat = ChoosePixelFormat(hDC, &pixelFormatDescriptor);
	SetPixelFormat(hDC, pixelformat, &pixelFormatDescriptor);

	HGLRC temp = wglCreateContext(hDC);
	wglMakeCurrent(hDC, temp);
	glewInit();

	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	HGLRC hRC = wglCreateContextAttribsARB(hDC, NULL, attributes);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(temp);
	wglMakeCurrent(hDC, hRC);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);

	RECT clientAreaRect;
	GetClientRect(hWnd, &clientAreaRect);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_LBUTTONDOWN:
		{
			float x = static_cast<float>(GET_X_LPARAM(lParam));
			float y = static_cast<float>(GET_Y_LPARAM(lParam));

			Vector2 screenCenter(
				static_cast<float>(screenWidth) * 0.5f,
				static_cast<float>(screenHeight) * 0.5f);

			Vector2 position(
				(x - screenCenter.x) / screenCenter.x,
				((y - screenCenter.y) / screenCenter.y) * -1.0f);

			if (fireworkPool)
				fireworkPool->launch(position);
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;

		case WM_DESTROY:
		{
			HDC hDC = GetDC(hWnd);
			hDC = wglGetCurrentDC();

			wglMakeCurrent(NULL, NULL);
			ReleaseDC(hWnd, hDC);
			wglDeleteContext(wglGetCurrentContext());

			PostQuitMessage(0);
		}
		return 0;

		case WM_SIZE:
			screenWidth = LOWORD(lParam);
			screenHeight = HIWORD(lParam);

			ResizeViewport(
				static_cast<int>(screenWidth),
				static_cast<int>(screenHeight));
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return 0;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	TCHAR windowName[48] = TEXT("Fireworks");
	TCHAR windowClassName[24] = TEXT("WindowClass");

	bool fullscreen = false;

	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpszClassName = windowClassName;
	windowClass.lpfnWndProc = (WNDPROC)::WndProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;

	windowClass.style = !fullscreen ?
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC :
		NULL;

	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;

	if (!::RegisterClassEx(&windowClass))
		return 0;

	::ShowCursor(TRUE);

	int windowWidth = 1024;
	int windowHeight = 768;

	HDC hDC = GetWindowDC(NULL);
	HWND hWnd = CreateWindowEx(
		!fullscreen ?
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_TOPMOST :
		WS_EX_TOPMOST,

		windowClassName,
		windowName,

		!fullscreen ?
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN :
		WS_POPUP,

		0,
		0,

		!fullscreen ?
		windowWidth :
		GetDeviceCaps(hDC, HORZRES),

		!fullscreen ?
		windowHeight :
		GetDeviceCaps(hDC, VERTRES),

		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	Initialize(hWnd);

	background = new Background();
	fireworkPool = new FireworkPool(1008.0f, 729.0f);

	MSG msg;
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return 0;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
			Render(hWnd);
	}

	UnregisterClass(windowClass.lpszClassName, hInstance);

	delete background;
	delete fireworkPool;

	return 0;
}
