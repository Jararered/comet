#include "Engine.h"

#include "Input/Input.h"

#include "Renderer/ViewCamera.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/TextureMap.h"

#include "Timer.h"
#include "Utilities.h"
#include "Profiler/Profiler.h"

#include <chrono>

using namespace Comet;

Engine::Engine()
{
    Profiler::Instance().Start();
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(0);

    m_Renderer.Initialize();
    m_Camera.Initialize();
    TextureMap::Initialize();
}

Engine::~Engine()
{
    StopPhysics();
    m_Renderer.Finalize();
    CloseWindow();
    Profiler::Instance().Stop();
}

void Engine::Initialize()
{
    StartPhysics();
    Update();
    StopPhysics();
}

void Engine::Update()
{
    while (!WindowShouldClose())
    {
        COMET_PROFILE_SCOPE("Engine::Frame", "render");
        PollInputEvents();

        {
            COMET_PROFILE_SCOPE("Engine::FrameUpdate", "main");
            m_EntityManager.FrameUpdate(Clock::Time());
        }

        Clock::Reset();

        {
            COMET_PROFILE_SCOPE("Engine::RendererUpdate", "render");
            m_Renderer.Update(m_LayerManager, m_Camera);
        }
        Profiler::Instance().FlushIfDue();
    }
}

void Engine::Finalize()
{
}

void Engine::StartPhysics()
{
    if (m_PhysicsRunning)
        return;

    m_PhysicsRunning = true;
    m_PhysicsThread = std::thread(&Engine::PhysicsLoop, this);
}

void Engine::StopPhysics()
{
    m_PhysicsRunning = false;

    if (m_PhysicsThread.joinable())
        m_PhysicsThread.join();
}

void Engine::PhysicsLoop()
{
    using clock = std::chrono::steady_clock;

    constexpr float fixedDt = 1.0f / 500.0f;
    constexpr auto tick = std::chrono::duration_cast<clock::duration>(std::chrono::duration<float>(fixedDt));

    auto nextTick = clock::now() + tick;
    while (m_PhysicsRunning)
    {
        COMET_PROFILE_SCOPE("Engine::PhysicsTick", "physics");
        m_EntityManager.PhysicsUpdate(fixedDt);
        std::this_thread::sleep_until(nextTick);
        nextTick += tick;

        const auto now = clock::now();
        if (nextTick < now)
            nextTick = now + tick;
    }

    Profiler::Instance().FlushIfDue();
}
