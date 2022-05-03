#pragma once

class MouseHandler;
class WindowHandler;

class KeyboardHandler
{
public:
    KeyboardHandler();
    ~KeyboardHandler();

    void SetupCallbacks();

    void AttachMouseHandler(MouseHandler *mouseHandler) { m_MouseHandler = mouseHandler; }

private:
    MouseHandler *m_MouseHandler;

    void KeyCallback(int key, int scancode, int action, int mods);
};
