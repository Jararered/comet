#pragma once

#include "Entities/EntityManager.h"
#include "Layer/LayerManager.h"
#include "Renderer/Renderer.h"

#include <raylib.h>

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
    Comet::ViewCamera& Camera() { return m_Camera; }

private:
    EntityManager m_EntityManager;
    LayerManager m_LayerManager;
    Comet::ViewCamera m_Camera;
    Renderer m_Renderer;
};

inline constexpr int WINDOW_WIDTH = 1280;
inline constexpr int WINDOW_HEIGHT = 720;
inline constexpr const char* WINDOW_TITLE = "Comet";
