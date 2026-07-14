#include "Engine.h"

#include "Entities/EntityManager.h"

#include "Input/Input.h"

#include "Renderer/ViewCamera.h"
#include "Renderer/Renderer.h"
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

    Renderer::Initialize();
    ViewCamera::Initialize();
    TextureMap::Initialize();
}

Engine::~Engine()
{
    Renderer::Finalize();
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

        EntityManager::FrameUpdate(Clock::Time());

        Clock::Reset();

        ViewCamera::Update();

        Renderer::Update();
    }
}

void Engine::Finalize()
{
}
