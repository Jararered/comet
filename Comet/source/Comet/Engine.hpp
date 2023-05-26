#pragma once

#include "Window.hpp"

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
