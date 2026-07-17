#pragma once

#include "Entities/EntityManager.h"
#include "Layer/LayerManager.h"
#include "Renderer/Renderer.h"

#include <atomic>
#include <raylib.h>
#include <thread>

class Engine
{
public:
    Engine();
    ~Engine();

    void Initialize();
    void Update();
    void Finalize();

    EntityManager& Entities() { return m_EntityManager; }
    LayerManager& Layers() { return m_LayerManager; }
    Renderer& GetRenderer() { return m_Renderer; }

private:
    void StartPhysics();
    void StopPhysics();
    void PhysicsLoop();

    EntityManager m_EntityManager;
    LayerManager m_LayerManager;
    Renderer m_Renderer;
    std::atomic_bool m_PhysicsRunning = false;
    std::thread m_PhysicsThread;
};

inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 720;
inline constexpr const char* WINDOW_TITLE = "Comet";
