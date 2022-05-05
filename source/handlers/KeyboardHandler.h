#pragma once

#include "MouseHandler.h"
#include "WindowHandler.h"

#include "glfw/glfw3.h"
class KeyboardHandler
{
public:
    inline static auto &Get()
    {
        static KeyboardHandler instance;
        return instance;
    }

    static void Initialize()
    {
        glfwSetKeyCallback(
            glfwGetCurrentContext(), [](GLFWwindow *window, int key, int scancode, int action, int mods) {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                {
                    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    glfwSetCursorPos(glfwGetCurrentContext(), 0.0, 0.0);
                }

                if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
                {
                    Get().m_PressedSpace = true;
                }
                if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
                {
                    Get().m_PressedSpace = false;
                }
            });
    }

private:
    KeyboardHandler() {}
    KeyboardHandler(KeyboardHandler const &);
    void operator=(KeyboardHandler const &);

public:
    static bool PressedSpace() { return Get().m_PressedSpace; }
    static bool HoldingSpace() { return Get().m_HoldingSpace; }

private:
    bool m_PressedSpace;
    bool m_HoldingSpace;
    bool m_CursorCaptured = false;
};
