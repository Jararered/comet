#include "Engine.h"

#include "Input/Input.h"

#include "Renderer/ViewCamera.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/TextureMap.h"

#include "Timer.h"
#include "Utilities.h"

using namespace Comet;

Engine::Engine()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(0);

    m_Renderer.Initialize();
    m_Camera.Initialize();
    TextureMap::Initialize();
}

Engine::~Engine()
{
    m_Renderer.Finalize();
    CloseWindow();
}

void Engine::Initialize()
{
    Update();
}

void Engine::Update()
{
    while (!WindowShouldClose())
    {
        PollInputEvents();

        m_EntityManager.FrameUpdate(Clock::Time());

        Clock::Reset();

        m_Renderer.Update(m_LayerManager, m_Camera);
    }
}

void Engine::Finalize()
{
}
