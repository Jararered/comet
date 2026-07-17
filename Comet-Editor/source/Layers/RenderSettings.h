#pragma once

#include <Layer/Layer.h>
#include <Layer/LayerManager.h>
#include <Renderer/Renderer.h>

#include "World/ChunkGenerator.h"
#include "World/World.h"

#include <memory>

class Settings : public Layer
{
public:
    Settings() = default;
    Settings(World* world, Renderer* renderer, const std::shared_ptr<Comet::ViewCamera>& camera) : m_World(world), m_Renderer(renderer), m_Camera(camera) {}
    ~Settings() = default;

    void Draw() override;

private:
    World* m_World = nullptr;
    Renderer* m_Renderer = nullptr;
    std::weak_ptr<Comet::ViewCamera> m_Camera;
};
