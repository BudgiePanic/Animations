#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <shellapi.h> // to process the command line arguments, if any
#include <functional> // to map command line arguments to demos
#include <direct.h> // to find the working directory
#include <iostream>

#include "glad.h"
#include "Application.h"
#include "demos/AnimationAdding.h"
#include "demos/AnimationBlending.h"
#include "demos/AnimatedModel.h"
#include "demos/CrossFadedAnimations.h"
#include "demos/DrawQuad.h"
#include "demos/DualQuaternionSkinning.h"
#include "demos/InverseKinematicsDemo.h"
#include "demos/SimpleAnimationPlayer.h"
#include "demos/WalkingDemo.h"

// Forward declare functions
// window entry function
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);
// window event processing function
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/// <summary>
/// check the command line argument for which demo to use
/// </summary>
/// <param name="args"></param>
/// <returns></returns>
Application* ProcessArgs();

/* #if _DEBUG */
// ~If debug build, connect to the console~ Always use the console, so we can get command line arguments
#pragma comment( linker, "/subsystem:console" )
int main(int argc, const char** argv) {
	return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
}

/* #else */
// otherwise connect to a window
/* #pragma comment( linker, "/subsystem:windows") */

/* #endif */
#pragma comment (lib, "opengl32.lib") // ask the linker to connect us to the opengl library

// Boilder plate to work with OpenGL
// taken from "wglext.h" from https://www.khronos.org/registry/OpenGL/index_gl.php 
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);
// taken from "wgl.h" from https://www.khronos.org/registry/OpenGL/index_gl.php 
typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

// GLOBAL VARIABLES
Application* gApplication = 0; // global_variable_pointer_application
GLuint gVertexArrayObject = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    gApplication = ProcessArgs();
	// the window
    WNDCLASSEX wndclass{};
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wndclass.lpszMenuName = 0;
    wndclass.lpszClassName = L"Win32 Game Window";
    RegisterClassEx(&wndclass);

    // Make sure the window is in the middle of the screen
    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int clientWidth = 800;
    const int clientHeight = 600;
    const int 
        left = (screenWidth / 2) - (clientWidth / 2),
        top = (screenHeight / 2) - (clientHeight / 2), 
        right = (screenWidth / 2) + (clientWidth / 2), 
        bottom = (screenHeight / 2) + (clientHeight / 2);
    RECT windowRect{};
    SetRect(&windowRect, left, top, right, bottom);
    DWORD windowStyle = (WS_OVERLAPPED, WS_CAPTION, WS_SYSMENU, WS_MINIMIZEBOX, WS_MAXIMIZEBOX);

    AdjustWindowRectEx(&windowRect, windowStyle, FALSE, 0);
    const int width = windowRect.right - windowRect.left, height = windowRect.bottom - windowRect.top;
    HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, L"Animation Window", windowStyle, 
        windowRect.left, windowRect.top, width, height, NULL, NULL, hInstance, szCmdLine);

    HDC hdc = GetDC(hwnd);

    // Create OpenGL context
    // find the correct pixel format then apply it to the device context of the window
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    // to get a pointer to wglCreateContextAttribsARB create a temporary OpenGL context
    HGLRC tempRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempRC);
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
    // to get a OpenGL 3.3 Core context profile, call the wglCreateContextAttribsARB function
    const int attribList[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,0,
    };
    HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempRC);
    wglMakeCurrent(hdc, hglrc);

    // use glad to initialize opengl functions
    if (!gladLoadGL()) {
        std::cout << "GLAD initialization failed\n";
    } else {
        std::cout << "OpenGL Version " << GLVersion.major << '.' << GLVersion.minor << '\n';
    }
    // enable Vsync
    // check if vsync is available
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
    const bool isVSyncSupported = strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;
    int vsynch = 0;
    if (isVSyncSupported) {
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
        PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");
        if (wglSwapIntervalEXT(1)) {
            std::cout << "Enabled vsynch\n";
            vsynch = wglGetSwapIntervalEXT();
        } else {
            std::cout << "Could not enable vsynch\n";
        }
    } else {
        std::cout << "WGL_EXT_swap_control not supported\n";
    }

    // tell the user the working directory where the program will search for resources
    char buffer[512]{};
    char* foundWD = _getcwd(buffer, 512);
    if (foundWD != NULL) {
        std::cout << "working dir is " << buffer << "\n";
    }

    // create and bind vertex array object
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // show the window
    bool resA = ShowWindow(hwnd, SW_SHOW);
    bool resB = UpdateWindow(hwnd);
    gApplication->Initialize();

    // application loop
    DWORD lastTick = GetTickCount(); // after ~49 days this will overflow
    MSG msg{};

    while (true) {
        // handle event messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break; // I'm outta here
            }
            TranslateMessage(&msg); // Convert keyboard input into localized data
            DispatchMessage(&msg);  // offload to event handler method
        }
        // update the 'application'
        DWORD tick = GetTickCount();
        float delta = float(tick - lastTick) * 0.001f;
        lastTick = tick;
        if (gApplication != 0) {
            gApplication->Update(delta);
        }
        // render with the 'application'
        if (gApplication != 0) {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int clientWidth = clientRect.right - clientRect.left;
            int clientHeight = clientRect.bottom - clientRect.top;
            glViewport(0, 0, clientWidth, clientHeight);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glPointSize(5.0f);
            glBindVertexArray(gVertexArrayObject);
            glClearColor(0.5f, 0.6f, 0.7f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            float aspect = (float)clientWidth /(float)clientHeight;
            gApplication->Render(aspect);
        }
        // issue buffer swap command to window
        if (gApplication != 0) {
            SwapBuffers(hdc);
            if (vsynch != 0) {
                glFinish();
            }
        }
    }
    // tidy up on exit
    if (gApplication != 0) {
        std::cout << "unexpected application not destroyed on exit, destroying now...\n";
        delete gApplication;
    }
    return (int)msg.wParam;
}

// Windows event handler function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {

    switch (iMsg) {
    case WM_CLOSE: 
        if (gApplication != 0) {
            gApplication->ShutDown();
            delete gApplication;
            gApplication = 0;
            DestroyWindow(hwnd);
        } else {
            std::cout << "you are attempting to close already shut down application\n";
        }
        break;
    case WM_DESTROY:
        if (gVertexArrayObject != 0) {
            // release openGL resources and window resources
            HDC hdc = GetDC(hwnd);
            HGLRC hglrc = wglGetCurrentContext();
            glBindVertexArray(0);
            glDeleteVertexArrays(1, &gVertexArrayObject);
            gVertexArrayObject = 0;
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(hglrc);
            ReleaseDC(hwnd, hdc);
            PostQuitMessage(0);
        } else {
            std::cout << "recieved multiple destroy messages\n";
        }
        break;
    case WM_PAINT: // ignore because OpenGL is managing the window
    case WM_ERASEBKGND:
        return 0;
    }
    // remaining cases are sent to default window event processor method
    return DefWindowProc(hwnd, iMsg, wParam, lParam); 
}

Application* ProcessArgs() {
    static const std::map<std::string, std::function<Application* ()>> argToDemo = {
        { "-animated_model", []() { return new demos::AnimatedModel(); }},
        { "-animation_adding", []() { return new demos::AnimationAdding(); }},
        { "-animation_blending", []() { return new demos::AnimationBlending(); }},
        { "-cross_fade_animation", []() { return new demos::CrossFadedAnimations(); }},
        { "-draw_quad", []() { return new demos::DrawQuad(); }},
        { "-dual_duaternion_skinning", []() { return new demos::DualQuaternionSkinning(); }},
        { "-inverse_kinematics", []() { return new demos::InverseKinematicsDemo(); }},
        { "-skeleton_wireframe", []() { return new demos::SimpleAnimationPlayer(); }},
        { "-inverse_kinematic_walking", []() { return new demos::WalkingDemo(); }},
    };
    // https://stackoverflow.com/questions/12460712/convert-lpwstr-to-string
    // https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw
    int numbArgs;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &numbArgs);
    if (args == NULL) {
        std::cout << "Could not parse program arguments\n";
        return new Application();
    }
    Application* answer = NULL;
    for (int i = 0; i < numbArgs; i++) {
        LPWSTR str = args[i];
        std::wstring wArg(str);
        std::string arg = std::string(wArg.begin(), wArg.end());
        if (argToDemo.find(arg) != argToDemo.end()) {
            answer = argToDemo.at(arg)();
            break; // accept the first match only
        }
    }
    LocalFree(args);
    if (answer == NULL) {
        answer = new Application();
        std::cout << "did not recognize any demos in argument list, loading default demo" << std::endl;
        std::cout << "valid arguments are:" << std::endl;
        for (auto const& entry : argToDemo) {
            std::cout << entry.first << std::endl;
        }
    }
    return answer;
}
