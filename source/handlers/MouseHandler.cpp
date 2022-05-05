#include "MouseHandler.h"
#include "glfw/glfw3.h"
#include "imgui.h"

void MouseHandler::Initialize() { Instance().SetupCallbacks(); }

void MouseHandler::SetupCallbacks()
{
    glfwSetWindowUserPointer(glfwGetCurrentContext(), &MouseHandler::Instance());

    glfwSetScrollCallback(glfwGetCurrentContext(),
        [](GLFWwindow *window, double xoffset, double yoffset) { Instance().m_ScrollOffset += yoffset; });

    glfwSetMouseButtonCallback(glfwGetCurrentContext(), [](GLFWwindow *window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            Instance().m_LeftClick = true;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            Instance().m_LeftClick = false;
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            Instance().m_RightClick = true;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            Instance().m_RightClick = false;
        }
    });

    glfwSetCursorPosCallback(glfwGetCurrentContext(), [](GLFWwindow *window, double xpos, double ypos) {
        // Only tracks cursor movement if the cursor is captured
        int mode = glfwGetInputMode(glfwGetCurrentContext(), GLFW_CURSOR);

        if (mode != GLFW_CURSOR_NORMAL)
        {
            // Adding up all movement since the last frame was rendered
            Instance().m_MovementSinceLastFrame[0] += xpos;
            Instance().m_MovementSinceLastFrame[1] -= ypos;

            // Setting cursor back so next callback is relative from center again
            glfwSetCursorPos(WindowHandler::Window(), 0.0, 0.0);
        }
        else
        {
            // No movement if not tracking cursor
            Instance().m_MovementSinceLastFrame = {0.0, 0.0};
        }
    });
}

void MouseHandler::CaptureCursor()
{
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(WindowHandler::Window(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetInputMode(WindowHandler::Window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Instance().m_CursorCaptured = true;

    // Need to center cursor before cursor position callback is run
    // Prevents a possibly large xpos/ypos when entering the window
    glfwSetCursorPos(WindowHandler::Window(), 0.0, 0.0);
}

void MouseHandler::ResetMovement() { Instance().m_MovementSinceLastFrame = {0.0, 0.0}; }
