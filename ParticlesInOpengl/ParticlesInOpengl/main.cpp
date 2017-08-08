
#include <windows.h>
#include <gl\glew.h>
#include <GL\wglew.h>
#include <gl\gl.h>
#include <vector>
#include "SpMatrix4x4.h"
#include <ctime>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib,"Winmm.lib")

#define NUM_PARTICLES   1000
#define PARTICLE_SIZE   7

#define ATTRIBUTE_LIFETIME_LOCATION       0
#define ATTRIBUTE_STARTPOSITION_LOCATION  1
#define ATTRIBUTE_ENDPOSITION_LOCATION    2

float* particleData1;

	/*
	const GLchar* vertexShaderSourceCode = "\
	#version 330 core\n\
	smooth out vec4 vSmoothColor;\n\
	uniform mat4 world;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	uniform float time;\n\
	const vec3 a = vec3(0, 2, 0);\n\
\n\
	const float rate = 1 / 500.0;\n\
	const float life = 2;\n\
\n\
	const float PI = 3.14159;\n\
	const float TWO_PI = 2 * PI;\n\
\n\
	const vec3 RED = vec3(1, 0, 0);\n\
	const vec3 GREEN = vec3(0, 1, 0);\n\
	const vec3 YELLOW = vec3(1, 1, 0);\n\
\n\
	float rand(vec2 co) {\n\
		return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);\n\
	}\n\
		\n\
	vec3 uniformRadomDir(vec2 v, out vec2 r) {\n\
		r.x = rand(v.xy);\n\
		r.y = rand(v.yx);\n\
		float theta = mix(0.0, PI / 6.0, r.x);\n\
		float phi = mix(0.0, TWO_PI, r.y);\n\
		return vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));\n\
	}\n\
\n\
	void main() {\n\
		vec3 pos = vec3(0);\n\
		float t = gl_VertexID*rate;\n\
		float alpha = 1;\n\
		if (time>t) {\n\
			float dt = mod((time - t), life);\n\
			vec2 xy = vec2(gl_VertexID, t);\n\
			vec2 rdm = vec2(0);\n\
			pos = ((uniformRadomDir(xy, rdm) + 0.5*a*dt)*dt);\n\
			alpha = 1.0 - (dt / life);\n\
		}\n\
\n\
		vSmoothColor = vec4(mix(RED, YELLOW, alpha), alpha);\n\
		gl_Position = projection * view * world * vec4(pos, 1);\n\
	}";
	*/

const GLchar* vertexShaderSourceCode =
	"#version 330 core                                    \n"
    "uniform float time;                                \n"
    "uniform vec3 u_centerPosition;                       \n"
    "layout(location = 0) in float a_lifetime;            \n"
    "layout(location = 1) in vec3 a_startPosition;        \n"
    "layout(location = 2) in vec3 a_endPosition;          \n"
    "out float v_lifetime;                                \n"
    "void main()                                          \n"
    "{                                                    \n"
    "  if ( u_time <= a_lifetime )                        \n"
    "  {                                                  \n"
    "    gl_Position.xyz = a_startPosition +              \n"
    "                      (time * a_endPosition);        \n"
    "    gl_Position.xyz += u_centerPosition;             \n"
    "    gl_Position.w = 1.0;                             \n"
    "  }                                                  \n"
    "  else                                               \n"
    "  {                                                  \n"
    "     gl_Position = vec4( -1000, -1000, 0, 0 );       \n"
    "  }                                                  \n"
    "  v_lifetime = 1.0 - ( time / a_lifetime );          \n"
    "  v_lifetime = clamp ( v_lifetime, 0.0, 1.0 );       \n"
    "  gl_PointSize = ( v_lifetime * v_lifetime ) * 40.0; \n"
	"}";

/*
	const GLchar* fragmentShaderSourceCode = "\
	#version 450 core\n\
	smooth in vec4 vSmoothColor;\
	layout(location = 0) out vec4 vFragColor;\
	void main() {\
		vFragColor = vSmoothColor;\
	}";
*/

const GLchar* fragmentShaderSourceCode =
	"#version 330 core                                    \n"
	"precision mediump float;                             \n"
	"in float v_lifetime;                                 \n"
	"layout(location = 0) out vec4 fragColor;             \n"
	"void main()                                          \n"
	"{                                                    \n"
	"  vec4 texColor;                                     \n"
	"  fragColor = vec4(0.3,0.5,0.8,1.0);				  \n"
	"  fragColor.a *= v_lifetime;                         \n"
	"}                                                    \n";


struct Vertex
{
	float lifeTime;
	float startPosition[3];
	float endPosition[3];
};

GLuint vertexArray;
GLuint vertexBuffer;
GLuint indexBuffer;

GLuint gpuProgram;
GLuint vertexShader;
GLuint fragmentShader;

GLint timeLoc;
GLint colorLoc;
GLint centerPositionLoc;
GLint samplerLoc;

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

	GLint success = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	::glCompileShader(fragmentShader);

	::glAttachShader(gpuProgram, vertexShader);
	::glAttachShader(gpuProgram, fragmentShader);

	//::glBindAttribLocation(gpuProgram, 0, "in_Position");
	glBindAttribLocation(gpuProgram, 0, "a_lifetime");
	glBindAttribLocation(gpuProgram, 1, "a_startPosition");
	glBindAttribLocation(gpuProgram, 2, "a_endPosition");

	::glLinkProgram(gpuProgram);

	::glDetachShader(gpuProgram, vertexShader);
	::glDetachShader(gpuProgram, fragmentShader);

	::glDeleteShader(vertexShader);
	::glDeleteShader(fragmentShader);
	::glUseProgram(gpuProgram);

	timeLoc = glGetUniformLocation(gpuProgram, "time");
	centerPositionLoc = glGetUniformLocation(gpuProgram, "u_centerPosition");

	float centerPosition[3] = { 0.0f,0.0f,0.0f };
	glUniform3fv(centerPositionLoc, 1, centerPosition);

	srand(0);

	particleData1 = new float[1000 * 7];
	for (int i = 0; i < NUM_PARTICLES; ++i)
	{
		float *particleData = &particleData1[i * 7];

		(*particleData++) = ((float)(rand() % 10000) / 10000.0f);
		(*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;
		(*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;
		(*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;
		(*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;
		(*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;
		(*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;
	}
}

void CreateBuffers()
{
	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexBuffer);

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*1000*7, particleData1, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 1, GL_FLOAT, false, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid*)sizeof(GLfloat));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid*)(sizeof(GLfloat)*4));
}

void UploadMatrices(HWND hWnd)
{
	SpMatrix4x4 view;
	SpMatrix4x4 projection;
	SpMatrix4x4 world;

	RECT clientAreaRect;
	::GetClientRect(hWnd, &clientAreaRect);

	view.lookAtLh(
		SpVector3(0.0f, 0.0f, 0.0f),
		SpVector3(0.0f, 0.0f, 1.0f),
		SpVector3(0.0f, 1.0f, 0.0f));

	projection.perspectiveProjectionFovLhOgl(
		45.0f * (3.14159265359f / 180.0f),
		static_cast<float>(clientAreaRect.right - clientAreaRect.left) /
		static_cast<float>(clientAreaRect.bottom - clientAreaRect.top),
		1.0f,
		100.0f);

	GLfloat worldArray[16] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 15.0f, 1.0f
	};
		
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

	//GLuint worldUniformLocation = glGetUniformLocation(gpuProgram, "world");
	//GLuint viewUniformLocation = glGetUniformLocation(gpuProgram, "view");
	//GLuint projectionUniformLocation = glGetUniformLocation(gpuProgram, "projection");

	//glUniformMatrix4fv(worldUniformLocation, 1, GL_FALSE, worldArray);
	//glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, viewArray);
	//glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, projectionArray);
}

float t = 0.0f;

void Render(HWND hWnd)
{
	if (t == 0.0f) {
		glUniform1f(glGetUniformLocation(gpuProgram, "time"), t);
		t = (float)clock();
	}
	else
	{
		t = ((float)clock() - t) / CLOCKS_PER_SEC;
		glUniform1f(glGetUniformLocation(gpuProgram, "time"), t);
	}

	::glClearColor(0, 0, 0, 0);
	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_POINTS, 0, 1000);
	::SwapBuffers(::GetDC(hWnd));

	++t;
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
	TCHAR windowName[48] = TEXT("SimpleGeometryWithViewProj");
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
	::UploadMatrices(hWnd);
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




///////////////////////////////////////////////////////////////////////////////////////////////
class ParticleSystem
{
public:
	void EmitShader();
	void DrawShader();
	void InitParticles();
	void InitModel();
	void Update();
	void EmitParticles(float deltaTime);
	void RenderParticles();

	GLint emitTimeLoc;
	GLint emitEmissionRateLoc;
	GLint emitNoiseSamplerLoc;
};

void ParticleSystem::EmitShader()
{
	program = ProgramManagerObj->ProgramLoad((char*) "TFEmit", VERTEX_SHADER_PRG_EMIT, FRAGMENT_SHADER_PRG_EMIT);

	glUseProgram(program->ProgramID);
	emitProgramObject = program->ProgramID;

	const char *feedbackVaryings[5] = { "position", "velocity", "size", "currentTime", "lifeTime" };

	glTransformFeedbackVaryings(emitProgramObject, 5, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(program);

	emitTimeLoc = GetUniform(program, "time");
	emitEmissionRateLoc = GetUniform(program, "emissionRate");
	emitNoiseSamplerLoc = GetUniform(program, "noiseTex");
}

void ParticleSystem::DrawShader()
{
	program = ProgramManagerObj->ProgramLoad((char*)"TFDraw", VERTEX_SHADER_PRG_DRAW, FRAGMENT_SHADER_PRG_DRAW);
	glUseProgram(program->ProgramID);

	MVP = GetUniform(program, (char*)"ModelViewProjectMatrix");

	drawProgramObject = program->ProgramID;

	drawTimeLoc = GetUniform(drawProgramObject, "time");
	drawColorLoc = GetUniform(drawProgramObject, "color");
	drawAccelerationLoc = GetUniform(program, "acceleration");
	samplerLoc = GetUniform(program, "tex");
}

void ParticleSystem::InitParticles()
{
	time = 0.0f;
	curSrcIndex = 0;
	textureId = image->getTextureID();

	if (textureId <= 0) { return; }

	noiseTextureId = Create3DNoiseTexture(128, 50.0);
	Particle particleData[NUM_PARTICLES];

	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		Particle *particle = &particleData[i];
		particle->position[0] = 0.0f;
		particle->position[1] = 0.0f;
		particle->velocity[0] = 0.0f;
		particle->velocity[1] = 0.0f;
		particle->size = 0.0f;
		particle->curtime = 0.0f;
		particle->lifetime = 0.0f;
	}

	glGenBuffers(2, &particleVBOs[0]);

	for (int i = 0; i < 2; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, particleVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * NUM_PARTICLES, particleData, GL_DYNAMIC_COPY);
	}
}

void ParticleSystem::InitModel()
{
	UpdateShader();
	DrawShader();
	InitParticles();
	Model::InitModel();
	return;
}

void ParticleSystem::Update()
{
	static clock_t lastTime = clock();
	clock_t currentTime = clock();
	float deltaTime = (currentTime - lastTime) / CLOCKS_PER_SEC*0.10;
	lastTime = currentTime;
	time += deltaTime;

	EmitParticles(deltaTime);
}

void ParticleSystem::EmitParticles(float deltaTime)
{
	GLuint srcVBO = particleVBOs[curSrcIndex];
	GLuint dstVBO = particleVBOs[(curSrcIndex + 1) % 2];

	glUseProgram(emitProgramObject);

	SetupVertexAttributes(srcVBO);

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, dstVBO);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, dstVBO);

	glEnable(GL_RASTERIZER_DISCARD);

	glUniform1f(emitTimeLoc, time);
	glUniform1f(emitEmissionRateLoc, EMISSION_RATE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noiseTextureId);
	glUniform1i(emitNoiseSamplerLoc, 0);

	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
	glEndTransformFeedback();

	emitSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	glDisable(GL_RASTERIZER_DISCARD);
	glUseProgram(0);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_3D, 0);

	curSrcIndex = (curSrcIndex + 1) % 2;
}

void ParticleSystem::RenderParticles()
{
	glWaitSync(emitSync, 0, GL_TIMEOUT_IGNORED);
	glDeleteSync(emitSync);
	glUseProgram(drawProgramObject);

	SetupVertexAttributes(particleVBOs[curSrcIndex]);
	glUniformMatrix4fv(MVP, 1, GL_FALSE, (float*) TransformObj->TransformGetModelViewProjectionMatrix());

	glUniform1f(drawTimeLoc, time);
	glUniform4f(drawColorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform2f(drawAccelerationLoc, 0.0f, ACCELERATION);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glUniform1i(samplerLoc, 0);
	glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
}