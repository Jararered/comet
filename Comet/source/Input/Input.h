#pragma once

#include "KeyboardKeyCodes.h"
#include "MouseButtonCodes.h"

#include <glfw/glfw3.h>
#include <glm/vec2.hpp>

namespace Input
{
    // Keyboard related functions
    inline static bool IsKeyPressed(int keycode)
    {
        int state = glfwGetKey(glfwGetCurrentContext(), keycode);
        return state == GLFW_PRESS;
    }

    // Mouse button related functions
    inline static bool IsMouseButtonPressed(int mouseButtonCode)
    {
        int state = glfwGetMouseButton(glfwGetCurrentContext(), mouseButtonCode);
        return state == GLFW_PRESS;
    }

    inline static bool IsLeftClick()
    {
        int state = glfwGetMouseButton(glfwGetCurrentContext(), MOUSE_BUTTON_LEFT);
        return state == GLFW_PRESS;
    }

    inline static bool IsRightClick()
    {
        int state = glfwGetMouseButton(glfwGetCurrentContext(), MOUSE_BUTTON_RIGHT);
        return state == GLFW_PRESS;
    }

    // Mouse position functions
    static glm::vec2 GetMousePosition()
    {
        double xpos, ypos;
        glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
        return {static_cast<float>(xpos), static_cast<float>(-ypos)};
    }

    // Mouse handling functions
    static bool IsMouseCaptured()
    {
        int option = glfwGetInputMode(glfwGetCurrentContext(), GLFW_CURSOR);
        return option == GLFW_CURSOR_DISABLED;
    }

    static glm::vec2 GetMouseMovement()
    {
        if (IsMouseCaptured())
        {
            glm::vec2 mousepos = GetMousePosition();
            glfwSetCursorPos(glfwGetCurrentContext(), 0.0, 0.0);
            return mousepos;
        }
        return {0.0f, 0.0f};
    }

    static void CaptureCursor()
    {
        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(glfwGetCurrentContext(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Need to center cursor before cursor position callback is run
        // Prevents a possibly large xpos/ypos when entering the window
        glfwSetCursorPos(glfwGetCurrentContext(), 0.0, 0.0);
    }

    static void ReleaseCursor()
    {
        // Place cursor in the center of the window once released
        glm::ivec2 size;
        glfwGetWindowSize(glfwGetCurrentContext(), &size.x, &size.y);
        glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPos(glfwGetCurrentContext(), static_cast<double>(size.x / 2), static_cast<double>(size.y / 2));
    }

    // Updates the current state of all inputs
    static void PollEvents()
    {
        glfwPollEvents();

        if (IsKeyPressed(KEY_ESCAPE))
        {
            Input::ReleaseCursor();
        }
    }
};
