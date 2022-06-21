#include "Engine.h"

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
    while (!Engine::IsShouldClose())
    {
        double physicsStartTime = Utilities::Clock::Time();
        EntityHandler::FrameUpdate(Utilities::Clock::Time());
        Utilities::Statistics::PhysicsTime = Utilities::Clock::Time() - physicsStartTime;

        // Reset clock after processing all Dt based events, mainly physics-related funtionality
        Utilities::Clock::Reset();

        // Update camera views for inputs
        Camera::Update();

        double renderTimeStart = Utilities::Clock::Time();
        // Clears color and depth buffers
        Renderer::NewFrame();
        // Drawing the mesh render queue
        Renderer::DrawMeshQueue();
        // Draw UI after everything else
        Renderer::DrawInterfaceQueue();
        // Swaps buffers to display new drawn frame
        Renderer::SwapBuffers();
        Utilities::Statistics::RenderTime = Utilities::Clock::Time() - renderTimeStart;

        // Poll events for next frame
        Input::PollEvents();
    }
}
