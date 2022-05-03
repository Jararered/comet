#include "WindowHandler.h"

#include "Engine.h"

WindowHandler::WindowHandler()
{
    CreateWindow();
    SetupCallbacks();
}

WindowHandler::~WindowHandler() {}

int WindowHandler::CreateWindow()
{
    // Initialize the library
    if (!glfwInit())
    {
        std::cout << "[Error] Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    // Create a windowed mode window and its OpenGL context
    m_Window = glfwCreateWindow(1, 1, "Comet (OpenGL 4.6)", NULL, NULL);
    if (!m_Window)
    {
        std::cout << "[Error] Failed to create OpenGL window.\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(m_Window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "[Error] Failed to initialize OpenGL context.\n";
        return -1;
    }

    glfwSetWindowUserPointer(m_Window, this);
    glfwSwapInterval(1);

    CenterWindow();

    glfwShowWindow(m_Window);

    return 0;
}

void WindowHandler::CenterWindow()
{
    int count;
    int monitorX, monitorY;

    GLFWmonitor **monitors = glfwGetMonitors(&count);
    const GLFWvidmode *videoMode = glfwGetVideoMode(monitors[0]);

    int newWindowWidth = int(videoMode->width / 1.5);
    int newWindowHeight = int(newWindowWidth / 16 * 9);

    SetWindowWidth(newWindowWidth);
    SetWindowHeight(newWindowHeight);

    glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);

    glfwSetWindowPos(glfwGetCurrentContext(), monitorX + (videoMode->width - newWindowWidth) / 2,
                     monitorY + (videoMode->height - newWindowHeight) / 2);
    glfwSetWindowSize(glfwGetCurrentContext(), newWindowWidth, newWindowHeight);
    glViewport(0, 0, newWindowWidth, newWindowHeight);
}

void WindowHandler::SetupCallbacks()
{
    glfwSetWindowUserPointer(glfwGetCurrentContext(), this);

    auto WindowSizeCallbackWrapper = [](GLFWwindow *window, int width, int height) {
        static_cast<WindowHandler *>(glfwGetWindowUserPointer(window))->WindowSizeCallback(width, height);
    };
    auto FrameBufferCallbackWrapper = [](GLFWwindow *window, int width, int height) {
        static_cast<WindowHandler *>(glfwGetWindowUserPointer(window))->FramebufferSizeCallback(width, height);
    };
    auto WindowCloseCallbackWrapper = [](GLFWwindow *window) {
        static_cast<WindowHandler *>(glfwGetWindowUserPointer(window))->WindowCloseCallback();
    };

    glfwSetWindowSizeCallback(glfwGetCurrentContext(), WindowSizeCallbackWrapper);
    glfwSetFramebufferSizeCallback(glfwGetCurrentContext(), FrameBufferCallbackWrapper);
    glfwSetWindowCloseCallback(glfwGetCurrentContext(), WindowCloseCallbackWrapper);
}

void WindowHandler::WindowSizeCallback(int width, int height) {}

void WindowHandler::FramebufferSizeCallback(int width, int height) { glViewport(0, 0, width, height); }

void WindowHandler::WindowCloseCallback() { m_Engine->SetShouldClose(true); }

bool WindowHandler::ShouldWindowClose() { return glfwWindowShouldClose(m_Window); }
