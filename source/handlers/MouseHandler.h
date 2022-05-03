#pragma once

#include <comet.pch>

#include "Engine.h"
#include "WindowHandler.h"

class MouseHandler
{
public:
    MouseHandler();
    ~MouseHandler();

    void AttachWindowhandler(WindowHandler *windowHandler) { m_WindowHandler = windowHandler; }

    void UpdateStates();
    void ResetStates();
    void SetupCallbacks();
    void ResetMovement();

    // +x is moving the mouse right
    // +y is moving the mouse forward
    void CaptureCursor();
    void ReleaseCursor();

private:
    GLFWwindow *p_GLFWwindow = nullptr;

    void ScrollCallback(double xoffset, double yoffset);
    void MouseButtonCallback(int button, int action, int mods);
    void CursorPosCallback(double xpos, double ypos);

    std::array<double, 2> m_MovementSinceLastFrame = {0.0, 0.0};
    bool m_CursorCaptured = false;
    bool m_LeftClick = false;
    bool m_LeftHold = false;
    bool m_RightClick = false;
    bool m_RightHold = false;

    double m_ScrollOffset = 0.0;

    WindowHandler *m_WindowHandler;

public:
    double ScrollOffset() { return m_ScrollOffset; }
    double DeltaX() { return m_MovementSinceLastFrame[0]; }
    double DeltaY() { return m_MovementSinceLastFrame[1]; }
    bool LeftClick() { return m_LeftClick; }
    bool LeftHold() { return m_LeftHold; }
    bool RightClick() { return m_RightClick; }
    bool RightHold() { return m_RightHold; }
};
