#include <Comet/Engine.hpp>

#include "World/Block.hpp"
#include "World/Chunk.hpp"
#include "World/World.hpp"

#include "Interfaces/Crosshair.hpp"
#include "Interfaces/RenderSettings.hpp"

#include "Entities/Player.hpp"

int main(int argc, char const *argv[])
{
    std::unique_ptr<Engine> engine = std::make_unique<Engine>();

    // Create player entity and add it to the entity handler
    // Entity handler is now on the world thread
    Player player;

    // Starting world thread and configuring
    World::Initialize();

    // Create the debugging menu
    RenderInterface renderInterface;
    CrosshairInterface crosshairInterface;

    // Starting main engine thread
    engine->Initialize();

    // Ending threads
    World::Finalize();

    // Engine is deconstructed
    return 0;
}
