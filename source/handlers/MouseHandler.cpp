#include "MouseHandler.h"
#include "imgui.h"

MouseHandler::MouseHandler() { SetupCallbacks(); }

MouseHandler::~MouseHandler() {}

void MouseHandler::SetupCallbacks()
{
    glfwSetWindowUserPointer(glfwGetCurrentContext(), this);

    auto ScrollCallbackWrapper = [](GLFWwindow *window, double xoffset, double yoffset) {
        static_cast<MouseHandler *>(glfwGetWindowUserPointer(window))->ScrollCallback(xoffset, yoffset);
    };
    auto MouseButtonCallbackWrapper = [](GLFWwindow *window, int button, int action, int mods) {
        static_cast<MouseHandler *>(glfwGetWindowUserPointer(window))->MouseButtonCallback(button, action, mods);
    };
    auto CursorPosCallbackWrapper = [](GLFWwindow *window, double xpos, double ypos) {
        static_cast<MouseHandler *>(glfwGetWindowUserPointer(window))->CursorPosCallback(xpos, ypos);
    };

    glfwSetScrollCallback(glfwGetCurrentContext(), ScrollCallbackWrapper);
    glfwSetMouseButtonCallback(glfwGetCurrentContext(), MouseButtonCallbackWrapper);
    glfwSetCursorPosCallback(glfwGetCurrentContext(), CursorPosCallbackWrapper);
}

void MouseHandler::ScrollCallback(double xoffset, double yoffset) { m_ScrollOffset += yoffset; }

void MouseHandler::MouseButtonCallback(int button, int action, int mods) {}

void MouseHandler::CursorPosCallback(double xpos, double ypos)
{
    // Only tracks cursor movement if the cursor is captured
    if (m_CursorCaptured)
    {
        // Adding up all movement since the last frame was rendered
        m_MovementSinceLastFrame[0] += xpos;
        m_MovementSinceLastFrame[1] -= ypos;

        // Setting cursor back so next callback is relative from center again
        glfwSetCursorPos(m_WindowHandler->Window(), 0.0, 0.0);
    }
    else
    {
        // No movement if not tracking cursor
        m_MovementSinceLastFrame = {0.0, 0.0};
    }
}

void MouseHandler::UpdateStates()
{
    if (glfwGetMouseButton(m_WindowHandler->Window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        if (!m_LeftClick && !m_LeftHold)
        {
            m_LeftClick = true;
        }
        else
        {
            m_LeftClick = false;
            m_LeftHold = true;
        }
    }
    else
    {
        m_LeftClick = false;
        m_LeftHold = false;
    }

    if (glfwGetMouseButton(m_WindowHandler->Window(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (!m_RightClick && !m_RightHold)
        {
            m_RightClick = true;
        }
        else
        {
            m_RightClick = false;
            m_RightHold = true;
        }
    }
    else
    {
        m_RightClick = false;
        m_RightHold = false;
    }
}

void MouseHandler::ResetStates()
{
    m_LeftClick = false;
    m_LeftHold = false;
    m_RightClick = false;
    m_RightHold = false;
}

void MouseHandler::CaptureCursor()
{
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(m_WindowHandler->Window(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetInputMode(m_WindowHandler->Window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_CursorCaptured = true;

    // Need to center cursor before cursor position callback is run
    // Prevents a possibly large xpos/ypos when entering the window
    glfwSetCursorPos(m_WindowHandler->Window(), 0.0, 0.0);
}
void MouseHandler::ReleaseCursor()
{
    glfwSetInputMode(m_WindowHandler->Window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_CursorCaptured = false;
}

void MouseHandler::ResetMovement() { m_MovementSinceLastFrame = {0.0, 0.0}; }
