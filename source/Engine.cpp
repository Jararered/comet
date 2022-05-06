#include "Engine.h"
#include "handlers/Input.h"

using namespace Comet;

void Engine::Initialize()
{
    Get().m_TimeDelta = 0.0;

    // Engine Components
    WindowHandler::Initialize();
    EventHandler::Initialize();

    // Rendering Components
    TextureMap::Initialize();
    Renderer::Initialize();
    Camera::Initialize();
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
        // Clears color and depth buffers
        Renderer::NewFrame();

        EntityHandler::FrameUpdate();

        // Update camera views for inputs
        Camera::Update();

        // Drawing the mesh render queue
        Renderer::DrawMeshQueue();
        // Draw UI after everything else
        Renderer::DrawInterfaceQueue();

        // Swaps buffers to display new drawn frame
        Renderer::SwapBuffers();

        // Poll events for next frame
        Input::PollEvents();

        Get().m_TimeDelta = glfwGetTime() - Get().m_TimeLast;
        Get().m_TimeLast = glfwGetTime();
    }
}
