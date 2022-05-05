#pragma once

#include "MouseHandler.h"
#include "WindowHandler.h"

class KeyboardHandler
{
public:
    inline static auto &Instance()
    {
        static KeyboardHandler instance;
        return instance;
    }

    static void Initialize();
    static void SetupCallbacks();

    static bool PressedSpace() {return Instance().m_PressedSpace;}
    static bool HoldingSpace() {return Instance().m_HoldingSpace;}

private:
    KeyboardHandler() {}
    KeyboardHandler(KeyboardHandler const &);
    void operator=(KeyboardHandler const &);

    void KeyCallback(int key, int scancode, int action, int mods);

    bool m_PressedSpace;
    bool m_HoldingSpace;
};
