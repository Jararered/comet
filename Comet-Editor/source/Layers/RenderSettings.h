#pragma once

#include <Layer/Layer.h>
#include <Layer/LayerManager.h>
#include <Renderer/Renderer.h>

#include "World/ChunkGenerator.h"
#include "World/World.h"

class Settings : public Layer
{
public:
    Settings() = default;
    Settings(World* world) : m_World(world) {}
    ~Settings() = default;

    void Draw() override;

private:
    World* m_World;
};
