#include "Engine.h"

void Engine::Initialize()
{
    Get().m_TimeDelta = 0.0;

    // Engine Components
    WindowHandler::Initialize();
    EventHandler::Initialize();
    KeyboardHandler::Initialize();
    MouseHandler::Initialize();

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
        // Reset accumulated movement
        MouseHandler::ResetMovement();

        // Drawing the mesh render queue
        Renderer::DrawMeshQueue();
        // Draw UI after everything else
        Renderer::DrawInterfaceQueue();



        // Swaps buffers to display new drawn frame
        Renderer::SwapBuffers();

        // Poll events for next frame
        EventHandler::PollEvents();



        Get().m_TimeDelta = glfwGetTime() - Get().m_TimeLast;
        Get().m_TimeLast = glfwGetTime();
    }
}
