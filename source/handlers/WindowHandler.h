#pragma once

#include <comet.pch>
#include <glad/gl.h>

#include "Engine.h"

class WindowHandler
{
public:
    inline static auto &Instance()
    {
        static WindowHandler instance;
        return instance;
    }

    static void Initialize();
    static void CenterWindow();
    static void SetupCallbacks();

    static bool ShouldWindowClose();

    int CreateWindow();

private:
    WindowHandler() {}
    WindowHandler(WindowHandler const &);
    void operator=(WindowHandler const &) {}

    void WindowSizeCallback(int width, int height);
    void FramebufferSizeCallback(int width, int height);
    void WindowCloseCallback();

    GLFWwindow *m_Window = nullptr;
    int m_WindowHeight = 0;
    int m_WindowWidth = 0;

public:
    static GLFWwindow *Window() { return Instance().m_Window; }

    static int WindowHeight() { return Instance().m_WindowHeight; }
    static void SetWindowHeight(int WindowHeight) { Instance().m_WindowHeight = WindowHeight; }
    static int WindowWidth() { return Instance().m_WindowWidth; }
    static void SetWindowWidth(int WindowWidth) { Instance().m_WindowWidth = WindowWidth; }
};
