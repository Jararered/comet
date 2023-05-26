#pragma once

#include <Comet/Renderer/Renderer.hpp>
#include <Comet/Interfaces/Interface.hpp>
#include <Comet/Interfaces/InterfaceManager.hpp>

#include "World/ChunkGenerator.hpp"
#include "World/World.hpp"

class RenderInterface : public Interface
{
public:
    RenderInterface();
    ~RenderInterface();

    void Update() override;

private:
    int m_RenderDistance;
    int m_RenderShape;
    int m_WorldSeed;
    glm::vec3 m_OverlayColor;
    glm::vec3 m_BackgroundColor;
};
