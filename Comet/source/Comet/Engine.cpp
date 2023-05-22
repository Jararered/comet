#include "Engine.hpp"

using namespace Comet;

void Engine::Initialize()
{
    // Engine Components
    Window::Initialize();
    Renderer::Initialize();
    Camera::Initialize();
    TextureMap::Initialize();
}

void Engine::Finalize()
{
    // Finalizing systems with threads
    glfwTerminate();
}

void Engine::Thread()
{
    while (!Window::ShouldClose())
    {
        double physicsStartTime = Clock::Time();
        EntityManager::FrameUpdate(Clock::Time());
        Statistics::PhysicsTime = Clock::Time() - physicsStartTime;

        // Reset clock after processing all Dt based events, mainly physics-related funtionality
        Clock::Reset();

        // Update camera views for inputs
        Camera::Update();

        double renderTimeStart = Clock::Time();

        Renderer::Update();

        Statistics::RenderTime = Clock::Time() - renderTimeStart;

        // Poll events for next frame
        Input::PollEvents();
    }
}
