#pragma once

#include <raylib.h>

class Engine
{
public:
    Engine();
    ~Engine();

    void Initialize();
    void Update();
    void Finalize();

private:
};

inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 720;
inline constexpr const char* WINDOW_TITLE = "Comet";
