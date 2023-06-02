#pragma once

#include "Window.h"

#include <memory>

class Engine
{
public:
    Engine();
    ~Engine();

    void Initialize();
    void Update();
    void Finalize();

private:
    std::unique_ptr<Window> m_Window;
};
