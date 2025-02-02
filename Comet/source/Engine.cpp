#include "Engine.h"

#include "Entities/EntityManager.h"

#include "Input/Input.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/TextureMap.h"

#include "Timer.h"
#include "Utilities.h"

using namespace Comet;

Engine::Engine()
{
    m_Window = std::make_unique<Window>();

    Renderer::Initialize();
    Camera::Initialize();
    TextureMap::Initialize();
}

Engine::~Engine()
{
    // Finalizing systems with threads
    glfwTerminate();
}

void Engine::Initialize()
{
    Update();
}

void Engine::Update()
{
    while (!m_Window->ShouldClose())
    {
        double physicsStartTime = Clock::Time();
        EntityManager::FrameUpdate(Clock::Time());

        // Reset clock after processing all Dt based events, mainly physics-related funtionality
        Clock::Reset();

        // Update camera views for inputs
        Camera::Update();

        double renderTimeStart = Clock::Time();

        m_Window->Update();

        // Poll events for next frame
        Input::PollEvents();
    }
}

void Engine::Finalize()
{
}