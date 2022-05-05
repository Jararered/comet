#include "MouseHandler.h"
#include "glfw/glfw3.h"
#include "imgui.h"

void MouseHandler::Initialize() { Get().SetupCallbacks(); }

void MouseHandler::SetupCallbacks()
{
    glfwSetScrollCallback(glfwGetCurrentContext(),
        [](GLFWwindow *window, double xoffset, double yoffset) { Get().m_ScrollOffset += yoffset; });

    glfwSetMouseButtonCallback(glfwGetCurrentContext(), [](GLFWwindow *window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            Get().m_LeftClick = true;
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            Get().m_LeftClick = false;

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            Get().m_RightClick = true;
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
            Get().m_RightClick = false;
    });

    glfwSetCursorPosCallback(glfwGetCurrentContext(), [](GLFWwindow *window, double xpos, double ypos) {
        // Only tracks cursor movement if the cursor is captured
        int cursorMode = glfwGetInputMode(glfwGetCurrentContext(), GLFW_CURSOR);

        if (cursorMode != GLFW_CURSOR_NORMAL)
        {
            // Adding up all movement since the last frame was rendered
            Get().m_MovementSinceLastFrame[0] += xpos;
            Get().m_MovementSinceLastFrame[1] -= ypos;

            // Setting cursor back so next callback is relative from center again
            glfwSetCursorPos(glfwGetCurrentContext(), 0.0, 0.0);
        }
        else
        {
            // No movement if not tracking cursor
            Get().m_MovementSinceLastFrame = {0.0, 0.0};
        }
    });
}

void MouseHandler::ResetMovement() { Get().m_MovementSinceLastFrame = {0.0, 0.0}; }
