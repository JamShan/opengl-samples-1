
#include <windows.h>
#include <gl\glew.h>
#include <gl\gl.h>

#pragma comment(lib, "opengl32.lib")

namespace
{
	void ResizeViewport(int width, int height)
	{
		::glViewport(0, 0, width, height);
	}

	void Render(HWND hWnd)
	{
		::glClearColor(0, 0, 1, 1);
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		::SwapBuffers(::GetDC(hWnd));
	}

	void Initialize(HWND hWnd)
	{
		HDC hDC = ::GetDC(hWnd);
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

		int pixelformat = ::ChoosePixelFormat(hDC, &pixelFormatDescriptor);
		::SetPixelFormat(hDC, pixelformat, &pixelFormatDescriptor);

		HGLRC hRC = ::wglCreateContext(hDC);
		::wglMakeCurrent(hDC, hRC);
		::glewInit();
		::glEnable(GL_DEPTH_TEST);

		RECT clientAreaRect;
		::GetClientRect(hWnd, &clientAreaRect);
		::glViewport(
			0,
			0,
			clientAreaRect.right - clientAreaRect.left,
			clientAreaRect.bottom - clientAreaRect.top);
	}

	void SwitchToFullscreen(HWND hWnd, DWORD screenWidth, DWORD screenHeight, DWORD bitsPerPixel)
	{
		::ShowWindow(hWnd, SW_HIDE);
		HDC hDC = ::GetWindowDC(NULL);

		DEVMODE devMode;
		::EnumDisplaySettings(NULL, 0, &devMode);

		devMode.dmPelsWidth = screenWidth;
		devMode.dmPelsHeight = screenHeight;
		devMode.dmBitsPerPel = bitsPerPixel;
		devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		::ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

		::SetClassLongPtr(hWnd, GCL_STYLE, NULL);
		::SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
		::SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);

		::SetWindowPos(
			hWnd,
			NULL,
			0,
			0,
			::GetDeviceCaps(hDC, HORZRES),
			::GetDeviceCaps(hDC, VERTRES),
			SWP_FRAMECHANGED);

		::ShowWindow(hWnd, SW_SHOWDEFAULT);
	}

	void SwitchToWindowed(HWND hWnd)
	{
		::ShowWindow(hWnd, SW_HIDE);
		::ChangeDisplaySettings(NULL, 0);

		::SetClassLongPtr(hWnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW | CS_OWNDC);
		::SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_TOPMOST);
		::SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		::SetWindowPos(hWnd, NULL, 0, 0, 800, 600, SWP_FRAMECHANGED);
		::ShowWindow(hWnd, SW_SHOWDEFAULT);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			::DestroyWindow(hWnd);
			return 0;

		case WM_DESTROY:
		{
			HDC hDC = ::GetDC(hWnd);
			hDC = ::wglGetCurrentDC();
			::wglMakeCurrent(NULL, NULL);
			::ReleaseDC(hWnd, hDC);
			::wglDeleteContext(wglGetCurrentContext());

			::PostQuitMessage(0);
		}
		return 0;

		case WM_SIZE:
			ResizeViewport(
				static_cast<int>(LOWORD(lParam)),
				static_cast<int>(HIWORD(lParam)));
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostMessage(hWnd, WM_CLOSE, 0, 0);
					return 0;

				case VK_UP:
					::SwitchToFullscreen(hWnd, 1366, 768, 32);
					break;

				case VK_DOWN:
					::SwitchToWindowed(hWnd);
					break;

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
	TCHAR windowName[24] = TEXT("OpenGLInitialization");
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

	int windowWidth = 800;
	int windowHeight = 600;

	HDC hDC = ::GetWindowDC(NULL);
	HWND hWnd = ::CreateWindowEx(
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
		::GetDeviceCaps(hDC, HORZRES),

		!fullscreen ?
		windowHeight :
		::GetDeviceCaps(hDC, VERTRES),

		NULL,
		NULL,
		hInstance,
		NULL);

	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::Initialize(hWnd);

	MSG msg;
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return 0;
			else
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
			Render(hWnd);
	}

	::UnregisterClass(windowClass.lpszClassName, hInstance);
	return 0;
}
