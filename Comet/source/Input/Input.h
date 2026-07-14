#pragma once

#include <raylib.h>
#include <glm/vec2.hpp>

namespace Input
{
    inline static bool s_IgnoreNextMouseDelta = false;
    inline static bool s_MouseCenterInitialized = false;

    inline static bool IsKeyPressed(int keycode)
    {
        return ::IsKeyDown(keycode);
    }

    inline static bool IsMouseButtonPressed(int mouseButtonCode)
    {
        return ::IsMouseButtonDown(mouseButtonCode);
    }

    inline static bool IsLeftClick()
    {
        return ::IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    }

    inline static bool IsRightClick()
    {
        return ::IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    }

    inline static float GetMouseWheelMove()
    {
        return ::GetMouseWheelMove();
    }

    static glm::vec2 GetMousePosition()
    {
        Vector2 pos = ::GetMousePosition();
        return {pos.x, -pos.y};
    }

    static bool IsMouseCaptured()
    {
        return ::IsCursorHidden();
    }

    static glm::vec2 GetMouseMovement()
    {
        if (IsMouseCaptured())
        {
            const int centerX = GetScreenWidth() / 2;
            const int centerY = GetScreenHeight() / 2;
            Vector2 position = ::GetMousePosition();

            if (!s_MouseCenterInitialized)
            {
                ::SetMousePosition(centerX, centerY);
                s_MouseCenterInitialized = true;
                s_IgnoreNextMouseDelta = false;
                return {0.0f, 0.0f};
            }

            glm::vec2 delta = {
                position.x - static_cast<float>(centerX),
                static_cast<float>(centerY) - position.y
            };

            ::SetMousePosition(centerX, centerY);

            if (s_IgnoreNextMouseDelta)
            {
                s_IgnoreNextMouseDelta = false;
                return {0.0f, 0.0f};
            }
            return delta;
        }
        return {0.0f, 0.0f};
    }

    static void CaptureCursor()
    {
        ::DisableCursor();
        s_IgnoreNextMouseDelta = true;
        s_MouseCenterInitialized = false;
    }

    static void ReleaseCursor()
    {
        ::EnableCursor();
        ::SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
        s_MouseCenterInitialized = false;
    }

    static void PollEvents()
    {
        if (::IsKeyPressed(KEY_ESCAPE) && IsMouseCaptured())
        {
            Input::ReleaseCursor();
        }
    }
};
