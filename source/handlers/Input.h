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

        static bool IsKeyPressed(int keycode)
        {
            int state = glfwGetKey(glfwGetCurrentContext(), keycode);
            return state == GLFW_PRESS;
        }

        static bool IsMouseButtonPressed(int mouseButtonCode)
        {
            int state = glfwGetMouseButton(glfwGetCurrentContext(), mouseButtonCode);
            return state = GLFW_PRESS;
        }

        static bool IsLeftClick()
        {
            int state = glfwGetMouseButton(glfwGetCurrentContext(), CT_MOUSE_BUTTON_LEFT);
            return state == GLFW_PRESS;
        }

        static bool IsRightClick()
        {
            int state = glfwGetMouseButton(glfwGetCurrentContext(), CT_MOUSE_BUTTON_RIGHT);
            return state == GLFW_PRESS;
        }

        static glm::vec2 GetMousePosition()
        {
            double xpos, ypos;
            glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);
            return {static_cast<float>(xpos), static_cast<float>(ypos)};
        }

    private:
        Input() {}
        Input(Input const &);
        void operator=(Input const &);
    };
} // namespace Comet
