#include "Engine.hpp"

#include "Entities/EntityManager.hpp"

#include "Input/Input.hpp"

#include "Renderer/Renderer.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/TextureMap.hpp"

#include "Timer.hpp"
#include "Utilities.hpp"

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

        Renderer::Update();

        // Poll events for next frame
        Input::PollEvents();
    }
}

void Engine::Finalize()
{
    
}