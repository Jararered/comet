#pragma once

#include "MouseHandler.h"
#include "WindowHandler.h"

#include "glfw/glfw3.h"
class KeyboardHandler
{
public:
    inline static auto &Instance()
    {
        static KeyboardHandler instance;
        return instance;
    }

    static void Initialize()
    {
        glfwSetWindowUserPointer(glfwGetCurrentContext(), &KeyboardHandler::Instance());

        glfwSetKeyCallback(
            glfwGetCurrentContext(), [](GLFWwindow *window, int key, int scancode, int action, int mods) {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                {
                    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    glfwSetCursorPos(glfwGetCurrentContext(), 0.0, 0.0);
                }

                if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
                {
                    Instance().m_PressedSpace = true;
                }
                if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
                {
                    Instance().m_PressedSpace = false;
                }
            });
    }

private:
    KeyboardHandler() {}
    KeyboardHandler(KeyboardHandler const &);
    void operator=(KeyboardHandler const &);

public:
    static bool PressedSpace() { return Instance().m_PressedSpace; }
    static bool HoldingSpace() { return Instance().m_HoldingSpace; }

private:
    bool m_PressedSpace;
    bool m_HoldingSpace;
    bool m_CursorCaptured = false;
};
