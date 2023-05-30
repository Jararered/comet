#pragma once

#include <Renderer/Renderer.hpp>
#include <Layer/Layer.hpp>
#include <Layer/LayerManager.hpp>

#include "World/ChunkGenerator.hpp"
#include "World/World.hpp"

class Settings : public Layer
{
public:
    Settings() = default;
    Settings(World* world) :m_World(world) {}
    ~Settings() = default;

    void Draw() override;

private:
    World* m_World;
};
