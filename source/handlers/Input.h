#pragma once

#include <comet.pch>

#include "WindowHandler.h"

#include "KeyboardKeyCodes.h"
#include "MouseButtonCodes.h"

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
            auto state = glfwGetKey(WindowHandler::Window(), keycode);
            return state == GLFW_PRESS;
        }
        static glm::vec2 GetMousePosition()
        {
            double xpos, ypos;
            glfwGetCursorPos(WindowHandler::Window(), &xpos, &ypos);
            return {(float)xpos, (float)ypos};
        }

    private:
        Input() {}
        Input(Input const &);
        void operator=(Input const &);
    };
} // namespace Comet
