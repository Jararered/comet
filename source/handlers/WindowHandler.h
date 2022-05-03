#pragma once

#include <comet.pch>
#include <glad/gl.h>

class Engine;
class WindowHandler
{
public:
    WindowHandler();
    ~WindowHandler();

    void AttachEngine(Engine *engine) { m_Engine = engine; }

    void CenterWindow();
    void SetupCallbacks();
    bool ShouldWindowClose();
    int CreateWindow();

private:
    Engine *m_Engine;

    void WindowSizeCallback(int width, int height);
    void FramebufferSizeCallback(int width, int height);
    void WindowCloseCallback();

    GLFWwindow *m_Window = nullptr;
    int m_WindowHeight = 0;
    int m_WindowWidth = 0;

public:
    GLFWwindow *Window() { return m_Window; }

    int WindowHeight() { return m_WindowHeight; }
    void SetWindowHeight(int WindowHeight) { m_WindowHeight = WindowHeight; }
    int WindowWidth() { return m_WindowWidth; }
    void SetWindowWidth(int WindowWidth) { m_WindowWidth = WindowWidth; }
};
