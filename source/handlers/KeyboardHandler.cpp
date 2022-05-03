#include "KeyboardHandler.h"

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
    // if (key == GLFW_KEY_HOME && action == GLFW_PRESS)
    // {
    //     WindowHandler::CenterWindow();
    // }
}
