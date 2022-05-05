#include "KeyboardHandler.h"
#include "glfw/glfw3.h"

void KeyboardHandler::Initialize() { Instance().SetupCallbacks(); }

void KeyboardHandler::SetupCallbacks()
{
    glfwSetWindowUserPointer(glfwGetCurrentContext(), &KeyboardHandler::Instance());

    auto KeyCallbackWrapper = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        static_cast<KeyboardHandler *>(glfwGetWindowUserPointer(window))->KeyCallback(key, scancode, action, mods);
    };

    glfwSetKeyCallback(glfwGetCurrentContext(), KeyCallbackWrapper);
}

void KeyboardHandler::KeyCallback(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        // glfwSetWindowShouldClose(glfwGetCurrentContext(), 1);
        MouseHandler::ReleaseCursor();
    }

    if (glfwGetKey(WindowHandler::Window(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!Instance().m_PressedSpace && !Instance().m_HoldingSpace)
        {
            Instance().m_PressedSpace = true;
        }
        else
        {
            Instance().m_PressedSpace = false;
            Instance().m_HoldingSpace = true;
        }
    }
    else
    {
        Instance().m_PressedSpace = false;
        Instance().m_HoldingSpace = false;
    }
}
