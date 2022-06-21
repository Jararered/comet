#include "Window.h"

void Window::Initialize()
{
    Get().CreateWindow();
    Get().SetupCallbacks();
}

int Window::CreateWindow()
{
    // Initialize the library
    if (!glfwInit())
    {
        std::cout << "[Error] Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    m_GLFWwindow = glfwCreateWindow(1, 1, "Comet (OpenGL 3.3)", NULL, NULL);
    if (!m_GLFWwindow)
    {
        std::cout << "[Error] Failed to create OpenGL window.\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(m_GLFWwindow);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "[Error] Failed to initialize OpenGL context.\n";
        return -1;
    }

    glfwSetWindowUserPointer(m_GLFWwindow, this);
    glfwSwapInterval(1);

    CenterWindow();

    return 0;
}

void Window::CenterWindow()
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

    glfwSetWindowPos(glfwGetCurrentContext(), monitorX + (videoMode->width - newWindowWidth) / 2, monitorY + (videoMode->height - newWindowHeight) / 2);
    glfwSetWindowSize(glfwGetCurrentContext(), newWindowWidth, newWindowHeight);
    glViewport(0, 0, newWindowWidth, newWindowHeight);
}

void Window::SetupCallbacks()
{
    glfwSetWindowSizeCallback(glfwGetCurrentContext(), [](GLFWwindow *window, int width, int height) {});
    glfwSetFramebufferSizeCallback(glfwGetCurrentContext(), [](GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); });
    glfwSetWindowCloseCallback(glfwGetCurrentContext(), [](GLFWwindow *window) { Engine::SetShouldClose(true); });
}

bool Window::CloseWindow()
{
    return glfwWindowShouldClose(Get().m_GLFWwindow);
}