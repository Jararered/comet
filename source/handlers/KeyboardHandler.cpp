#include "KeyboardHandler.h"

#include "MouseHandler.h"
#include "WindowHandler.h"

KeyboardHandler::KeyboardHandler() { SetupCallbacks(); }

KeyboardHandler::~KeyboardHandler() {}

void KeyboardHandler::SetupCallbacks()
{
    glfwSetWindowUserPointer(glfwGetCurrentContext(), this);

    auto KeyCallbackWrapper = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        static_cast<KeyboardHandler *>(glfwGetWindowUserPointer(window))->KeyCallback(key, scancode, action, mods);
    };

    glfwSetKeyCallback(glfwGetCurrentContext(), KeyCallbackWrapper);
}

void KeyboardHandler::KeyCallback(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        m_MouseHandler->ReleaseCursor();
    }
}
