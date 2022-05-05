#pragma once

#include <comet.pch>

#include "KeyboardKeyCodes.h"
#include "MouseButtonCodes.h"

#include "glfw/glfw3.h"

namespace Comet
{
    class Input
    {
    public:
        inline static auto &Get()
        {
            static Input instance;
            return instance;
        }

        inline static bool IsKeyPressed(int keycode)
        {
            return glfwGetKey(glfwGetCurrentContext(), keycode) == GLFW_PRESS;
        }

        inline static bool IsMouseButtonPressed(int mouseButtonCode)
        {
            return glfwGetMouseButton(glfwGetCurrentContext(), mouseButtonCode) == GLFW_PRESS;
        }

        inline static bool IsLeftClick()
        {
            return glfwGetMouseButton(glfwGetCurrentContext(), CT_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        }

        inline static bool IsRightClick()
        {
            return glfwGetMouseButton(glfwGetCurrentContext(), CT_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        }

        static glm::vec2 GetMousePosition()
        {
            double xpos, ypos;
            glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
            return {static_cast<float>(xpos), static_cast<float>(ypos)};
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

    private:
        Input() {}
        Input(Input const &);
        void operator=(Input const &);
    };
} // namespace Comet
