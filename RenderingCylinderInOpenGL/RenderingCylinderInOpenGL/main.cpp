#include <windows.h>
#include <windowsx.h>
#include <gl\glew.h>
#include <gl\wglew.h>
#include <gl\gl.h>
#include <vector>

#include "SpQuaternion.h"
#include "SpMatrix4x4.h"
#include "SpVector3.h"

#include "Camera.h"

#pragma comment(lib, "opengl32.lib")

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT) 0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT) 0x02)
#endif

namespace
{
	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint indexBuffer;

	Camera camera;

	struct Vertex
	{
		float x, y, z;
	};

	struct Triangle
	{
		GLuint v1, v2, v3;
	};

	struct Quadrilateral
	{
		Triangle t1, t2;
	};

	const float cylinderRadius = 10.0f;
	const float cylinderHeight = 100.0f;
	const float sector = (3.14159265358979f * 2.0f) / 400;

	void generateCylinderVertices(size_t numVertices = 802)
	{
		size_t numVerticesExcludingCenters = numVertices - 2;
		float angularPosition = 0.0f;
		Vertex* vertices = new Vertex[numVertices];

		Vertex& first = vertices[0];
		first.x = 0.0f;
		first.y = cylinderHeight / 2.0f;
		first.z = 0.0f;

		size_t i = 1;

		while (i < (numVerticesExcludingCenters / 2) + 1)
		{
			Vertex& vertex = vertices[i];
			vertex.y = cylinderHeight / 2.0f;
			vertex.x = cylinderRadius * cosf(angularPosition);
			vertex.z = cylinderRadius * sinf(angularPosition);

			angularPosition -= sector;
			++i;
		}

		Vertex& last = vertices[(numVerticesExcludingCenters / 2) + 1];
		last.x = 0.0f;
		last.y = -cylinderHeight / 2.0f;
		last.z = 0.0f;

		angularPosition = 0.0f;
		i = (numVerticesExcludingCenters/2) + 2;

		while (i < numVertices)
		{
			Vertex& vertex = vertices[i];

			vertex.y = -cylinderHeight / 2.0f;
			vertex.x = cylinderRadius * cosf(angularPosition);
			vertex.z = cylinderRadius * sinf(angularPosition);

			angularPosition += sector;
			++i;
		}

		::glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW);
		delete[] vertices;

		::glEnableVertexAttribArray(0);
		::glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
	}

	/* CLOCKWISE */
	void generateCircleIndices(size_t numVertices, size_t centerIndex, size_t indexOfFirstVertexOnCircle, size_t offset)
	{
		size_t numVerticesExcludingCenter = numVertices - 1;
		size_t numTriangles = numVerticesExcludingCenter;

		size_t firstTriangle = 0;
		size_t lastTriangle = numVerticesExcludingCenter - 1;

		Triangle* triangles = new Triangle[numTriangles];

		Triangle& first = triangles[firstTriangle];
		first.v1 = centerIndex;
		first.v2 = indexOfFirstVertexOnCircle;
		first.v3 = indexOfFirstVertexOnCircle + 1;

		Triangle& last = triangles[lastTriangle];
		last.v1 = centerIndex;
		last.v2 = indexOfFirstVertexOnCircle + numVerticesExcludingCenter - 1;
		last.v3 = indexOfFirstVertexOnCircle;

		size_t i = 1;
		size_t v = indexOfFirstVertexOnCircle + 1;

		while (i < numTriangles - 1)
		{
			Triangle& triangle = triangles[i];
			triangle.v1 = centerIndex;
			triangle.v2 = v++;
			triangle.v3 = v;

			++i;
		}

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, sizeof(Triangle) * numTriangles, triangles);
		delete[]triangles;
	}

	void generateCylinderSideIndices(size_t numVertices = 802, size_t indexOfFirstVertexOnTopCircle = 1, size_t indexOfFirstVertexOnBottomCircle = 400 + 2)
	{
		size_t numVerticesExcludingCenters = numVertices - 2;
		size_t numQuadrilaterals = numVerticesExcludingCenters / 2;
		Quadrilateral* quadrilaterals = new Quadrilateral[numQuadrilaterals];

		size_t topEndIndex =
			indexOfFirstVertexOnTopCircle +
			(numVerticesExcludingCenters / 2) - 1;

		Quadrilateral& first = quadrilaterals[0];
		first.t1.v1 = indexOfFirstVertexOnBottomCircle;
		first.t1.v2 = indexOfFirstVertexOnTopCircle;
		first.t1.v3 = topEndIndex;

		first.t2.v1 = indexOfFirstVertexOnBottomCircle;
		first.t2.v2 = topEndIndex;
		first.t2.v3 = indexOfFirstVertexOnBottomCircle + 1;

		size_t topIndex = topEndIndex;
		size_t bottomIndex = indexOfFirstVertexOnBottomCircle + 1;
		size_t q = 1;

		while (q < ((numVerticesExcludingCenters/2) - 1))
		{
			Quadrilateral& quadrilateral = quadrilaterals[q];
			quadrilateral.t1.v1 = bottomIndex;
			quadrilateral.t1.v2 = topIndex;
			quadrilateral.t1.v3 = topIndex - 1;

			quadrilateral.t2.v1 = bottomIndex;
			quadrilateral.t2.v2 = topIndex - 1;
			quadrilateral.t2.v3 = bottomIndex + 1;

			--topIndex;
			++bottomIndex;
			++q;
		}

		Quadrilateral& last = quadrilaterals[numQuadrilaterals - 1];
		last.t1.v1 = indexOfFirstVertexOnBottomCircle + ((numVerticesExcludingCenters / 2) - 1);
		last.t1.v2 = indexOfFirstVertexOnTopCircle + 1;
		last.t1.v3 = indexOfFirstVertexOnTopCircle;

		last.t2.v1 = indexOfFirstVertexOnBottomCircle + ((numVerticesExcludingCenters / 2) - 1);
		last.t2.v2 = indexOfFirstVertexOnTopCircle;
		last.t2.v3 = indexOfFirstVertexOnBottomCircle;

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (numQuadrilaterals * 2 * 3) / 2, sizeof(Triangle) * numQuadrilaterals * 2, quadrilaterals);
	}

	void generateCylinder()
	{
		glGenVertexArrays(1, &vertexArray);
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &indexBuffer);

		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		generateCylinderVertices();

		const size_t numVerticesPerCircle = 400;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (numVerticesPerCircle * 3 * 4), 0, GL_STATIC_DRAW);
		generateCircleIndices(401, 0, 1, 0);
		generateCylinderSideIndices();
		generateCircleIndices(401, 401, 402, sizeof(GLuint) * numVerticesPerCircle * 3 * 3);
	}

	const GLchar* vertexShaderSourceCode = "\
		#version 450 core\n\
		uniform mat4 view;\n\
		uniform mat4 projection;\n\
		in vec3 in_Position;\n\
		void main() {\n\
			gl_Position = projection * view * vec4(in_Position.xyz, 1.0);\n\
		}";

	const GLchar* fragmentShaderSourceCode = "\
		#version 450 core\n\
		out vec4 colorOut;\n\
		void main() {\n\
			colorOut = vec4(0.2, 0.2, 7.0, 1.0);\n\
		}";


	GLuint gpuProgram;
	GLuint vertexShader;
	GLuint fragmentShader;

	void ResizeViewport(int width, int height)
	{
		::glViewport(0, 0, width, height);
	}

	void InstallGpuProgram()
	{
		gpuProgram = glCreateProgram();
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShader, 1, &vertexShaderSourceCode, NULL);
		glShaderSource(fragmentShader, 1, &fragmentShaderSourceCode, NULL);

		glCompileShader(vertexShader);
		glCompileShader(fragmentShader);

		glAttachShader(gpuProgram, vertexShader);
		glAttachShader(gpuProgram, fragmentShader);

		glBindAttribLocation(gpuProgram, 0, "in_Position");
		glLinkProgram(gpuProgram);

		glDetachShader(gpuProgram, vertexShader);
		glDetachShader(gpuProgram, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void UploadMatrices(HWND hWnd)
	{
		SpMatrix4x4 view;
		SpMatrix4x4 projection;

		RECT clientAreaRect;
		::GetClientRect(hWnd, &clientAreaRect);

		view = camera.getViewMatrix();

		projection.perspectiveProjectionFovLhOgl(
			90.0f * (3.14159265359f / 180.0f),
			static_cast<float>(clientAreaRect.right - clientAreaRect.left) /
			static_cast<float>(clientAreaRect.bottom - clientAreaRect.top),
			1.0f,
			1000.0f);
		
		GLfloat viewArray[16] =
		{
			view.m11, view.m12, view.m13, view.m14,
			view.m21, view.m22, view.m23, view.m24,
			view.m31, view.m32, view.m33, view.m34,
			view.m41, view.m42, view.m43, view.m44,
		};

		GLfloat projectionArray[16] =
		{
			projection.m11, projection.m12, projection.m13, projection.m14,
			projection.m21, projection.m22, projection.m23, projection.m24,
			projection.m31, projection.m32, projection.m33, projection.m34,
			projection.m41, projection.m42, projection.m43, projection.m44,
		};

		GLuint viewUniformLocation = glGetUniformLocation(gpuProgram, "view");
		GLuint projectionUniformLocation = glGetUniformLocation(gpuProgram, "projection");

		glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, viewArray);
		glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, projectionArray);
	}

	void Render(HWND hWnd)
	{
		glEnable(GL_POLYGON_MODE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(gpuProgram);
		UploadMatrices(hWnd);
		glBindVertexArray(vertexArray);
		glDrawElements(GL_TRIANGLES, (400 * 3) + (800 * 3) + (400 * 3), GL_UNSIGNED_INT, 0);

		SwapBuffers(GetDC(hWnd));
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
		::glEnable(GL_CULL_FACE);

		::glCullFace(GL_BACK);
		::glFrontFace(GL_CW);

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

				case VK_SPACE:
					::SwitchToFullscreen(hWnd, 1366, 768, 32);
					break;

				case VK_TAB:
					//::SwitchToWindowed(hWnd);
					break;

				case VK_UP:
					::camera.forward(2.0f);
					break;

				case VK_DOWN:
					::camera.backward(2.0f);
					break;

				case VK_LEFT:
					::camera.left(2.0f);
					break;

				case VK_RIGHT:
					::camera.right(2.0f);
					break;

				case VK_CONTROL:
					::camera.roll(2.0f);
					break;

				case VK_DELETE:
					::camera.roll(-2.0f);
					break;

				default:
					break;
			}
			break;

		case WM_INPUT:
		{
			UINT dwSize = 40;
			static BYTE lpb[40];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
				lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				camera.yaw(0.1f * static_cast<float>(raw->data.mouse.lLastX));
				camera.pitch(0.1f * static_cast<float>(raw->data.mouse.lLastY));
			}
			break;
		}

		default:
			break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	TCHAR windowName[48] = TEXT("Cylinder Generator");
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

	::ShowCursor(FALSE);

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

	generateCylinder();

	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hWnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

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
