
#include <windows.h>
#include <gl\glew.h>
#include <GL\wglew.h>
#include <gl\gl.h>
#include <vector>

#pragma comment(lib, "opengl32.lib")

struct Vertex
{
	GLfloat x, y, z;
};

const GLchar* vertexShaderSourceCode = "\
#version 450 core\n\
in vec3 in_Position;\n\
void main() {\n\
	gl_Position = vec4(in_Position, 1.0);\n\
}";

const GLchar* fragmentShaderSourceCode = "\
#version 450 core\n\
out vec4 colorOut;\n\
void main() {\n\
	colorOut = vec4(1.0, 0.0, 0.0, 0.5);\n\
}";

namespace
{
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint indexBuffer;

	GLuint gpuProgram;
	GLuint vertexShader;
	GLuint fragmentShader;

	void ResizeViewport(int width, int height)
	{
		::glViewport(0, 0, width, height);
	}

	void InstallGpuProgram()
	{
		gpuProgram = ::glCreateProgram();
		vertexShader = ::glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = ::glCreateShader(GL_FRAGMENT_SHADER);

		::glShaderSource(vertexShader, 1, &vertexShaderSourceCode, NULL);
		::glShaderSource(fragmentShader, 1, &fragmentShaderSourceCode, NULL);

		::glCompileShader(vertexShader);
		::glCompileShader(fragmentShader);

		::glAttachShader(gpuProgram, vertexShader);
		::glAttachShader(gpuProgram, fragmentShader);

		::glBindAttribLocation(gpuProgram, 0, "in_Position");
		::glLinkProgram(gpuProgram);

		::glDetachShader(gpuProgram, vertexShader);
		::glDetachShader(gpuProgram, fragmentShader);

		::glDeleteShader(vertexShader);
		::glDeleteShader(fragmentShader);
		::glUseProgram(gpuProgram);
	}

	void CreateBuffers()
	{
		Vertex vertices[3] =
		{
			{0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}
		};

		GLuint indices[3] = { 0,1,2 };

		::glGenVertexArrays(1, &vertexArray);
		::glGenBuffers(1, &vertexBuffer);
		::glGenBuffers(1, &indexBuffer);

		::glBindVertexArray(vertexArray);
		::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		::glEnableVertexAttribArray(0);
		::glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
	}

	void Render(HWND hWnd)
	{
		::glClearColor(0, 0, 1, 1);
		::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		::glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
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

		HGLRC temp = ::wglCreateContext(hDC);
		::wglMakeCurrent(hDC, temp);
		::glewInit();

		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};

		HGLRC hRC = ::wglCreateContextAttribsARB(hDC, NULL, attributes);
		::wglMakeCurrent(NULL, NULL);
		::wglDeleteContext(temp);
		::wglMakeCurrent(hDC, hRC);

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
			::glBindVertexArray(vertexArray);
			::glBindBuffer(GL_ARRAY_BUFFER, 0);
			::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			::glDeleteBuffers(1, &vertexBuffer);
			::glDeleteBuffers(1, &indexBuffer);
			::glBindVertexArray(0);

			::glUseProgram(0);
			::glDeleteProgram(gpuProgram);

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
	::InstallGpuProgram();
	::CreateBuffers();

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
