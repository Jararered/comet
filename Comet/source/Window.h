#pragma once

struct GLFWwindow;
class Renderer;

class Window
{
public:
    Window();
    ~Window();

    void Initialize();
    void Update();
    void Center();
    bool CloseWindow();

private:
    GLFWwindow* m_GLFWwindow = nullptr;
    int m_WindowHeight = 0;
    int m_WindowWidth = 0;

public:
    GLFWwindow* GetGLFWwindow() { return m_GLFWwindow; }
    Renderer* m_Renderer;

    int WindowHeight() { return m_WindowHeight; }
    void SetWindowHeight(int WindowHeight) { m_WindowHeight = WindowHeight; }
    int WindowWidth() { return m_WindowWidth; }
    void SetWindowWidth(int WindowWidth) { m_WindowWidth = WindowWidth; }

    bool ShouldClose();
    void SetShouldClose(bool flag);
};
