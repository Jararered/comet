#pragma once

#include <comet.pch>

#include "Renderer.h"
#include "handlers/Interface.h"
#include "handlers/InterfaceHandler.h"

#include "imgui.h"
#include "world/ChunkGenerator.h"
#include "world/World.h"

class RenderInterface : public Interface
{
  public:
    RenderInterface();
    ~RenderInterface();

    void Draw() override;

  private:
    int m_RenderDistance;
    int m_RenderShape;
    int m_WorldSeed;
    glm::vec3 m_OverlayColor;
    glm::vec3 m_BackgroundColor;
};
