#pragma once

#include <comet.pch>
#include <glad/gl.h>

#include "Engine.h"

class WindowHandler
{
public:
    inline static auto &Get()
    {
        static WindowHandler instance;
        return instance;
    }

    static void Initialize();
    static void CenterWindow();
    static void SetupCallbacks();

    static bool CloseWindow();

    int CreateWindow();

private:
    WindowHandler() {}
    WindowHandler(WindowHandler const &);
    void operator=(WindowHandler const &) {}

    void WindowSizeCallback(int width, int height);
    void FramebufferSizeCallback(int width, int height);
    void WindowCloseCallback();

    GLFWwindow *m_GLFWwindow = nullptr;
    int m_WindowHeight = 0;
    int m_WindowWidth = 0;

public:
    static GLFWwindow *Window() { return Get().m_GLFWwindow; }

    static int WindowHeight() { return Get().m_WindowHeight; }
    static void SetWindowHeight(int WindowHeight) { Get().m_WindowHeight = WindowHeight; }
    static int WindowWidth() { return Get().m_WindowWidth; }
    static void SetWindowWidth(int WindowWidth) { Get().m_WindowWidth = WindowWidth; }
};
