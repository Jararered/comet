#pragma once

#include <glad/gl.h>
#include <glfw/glfw3.h>

#include "Engine.hpp"

class Window
{
public:
    inline static auto& Get()
    {
        static Window instance;
        return instance;
    }

    static void Initialize();
    static void CenterWindow();
    static bool CloseWindow();

private:
    Window() {}
    Window(Window const&);
    void operator=(Window const&) {}

    inline static GLFWwindow* m_GLFWwindow = nullptr;
    inline static int m_WindowHeight = 0;
    inline static int m_WindowWidth = 0;

public:
    static GLFWwindow* GetGLFWwindow() { return Get().m_GLFWwindow; }

    static int WindowHeight() { return Get().m_WindowHeight; }
    static void SetWindowHeight(int WindowHeight) { Get().m_WindowHeight = WindowHeight; }
    static int WindowWidth() { return Get().m_WindowWidth; }
    static void SetWindowWidth(int WindowWidth) { Get().m_WindowWidth = WindowWidth; }

    static bool ShouldClose() { return glfwWindowShouldClose(m_GLFWwindow); }
    static void SetShouldClose(bool flag) { glfwSetWindowShouldClose(m_GLFWwindow, flag); }

};
