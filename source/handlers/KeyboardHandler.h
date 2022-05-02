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

private:
    KeyboardHandler() {}
    KeyboardHandler(KeyboardHandler const &);
    void operator=(KeyboardHandler const &);

    void KeyCallback(int key, int scancode, int action, int mods);
};
