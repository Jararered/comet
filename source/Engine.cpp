#include "Engine.h"

#include "handlers/EntityHandler.h"
#include "handlers/ErrorHandler.h"
#include "handlers/EventHandler.h"
#include "handlers/InterfaceHandler.h"
#include "handlers/KeyboardHandler.h"
#include "handlers/MouseHandler.h"

Engine::Engine()
{
    m_TimeDelta = 0.0;

    // Engine Components
    m_WindowHandler = new WindowHandler();
    m_EventHandler = new EventHandler();
    m_KeyboardHandler = new KeyboardHandler();
    m_InterfaceHandler = new InterfaceHandler();
    m_MouseHandler = new MouseHandler();
    m_ErrorHandler = new ErrorHandler();
    m_EntityHandler = new EntityHandler();

    m_KeyboardHandler->AttachMouseHandler(m_MouseHandler);
    m_MouseHandler->AttachWindowhandler(m_WindowHandler);
    m_WindowHandler->AttachEngine(this);

    // Rendering Components
    m_TextureMap = new TextureMap();
    m_Renderer = new Renderer();
    m_Camera = new Camera();
}

Engine::~Engine()
{
    // Engine Components
    delete m_WindowHandler;
    delete m_EventHandler;
    delete m_KeyboardHandler;
    delete m_InterfaceHandler;
    delete m_MouseHandler;
    delete m_ErrorHandler;
    delete m_EntityHandler;

    delete m_TextureMap;
    delete m_Renderer;
    delete m_Camera;

    // Finalizing systems with threads
    glfwTerminate();
}

void Engine::Thread()
{
    while (!Engine::IsShouldClose())
    {
        // Clears color and depth buffers
        m_Renderer->NewFrame();

        // Update all handlers
        m_MouseHandler->UpdateStates();

        m_EntityHandler->FrameUpdate();

        // Update camera views for inputs
        m_Camera->Update();
        // Reset accumulated movement
        m_MouseHandler->ResetMovement();

        // Drawing the mesh render queue
        m_Renderer->DrawMeshQueue();
        // Draw UI after everything else
        m_Renderer->DrawInterfaceQueue();

        // Swaps buffers to display new drawn frame
        m_Renderer->SwapBuffers();
        // Poll events for next frame
        m_EventHandler->PollEvents();

        m_TimeDelta = glfwGetTime() - m_TimeLast;
        m_TimeLast = glfwGetTime();
    }
}
