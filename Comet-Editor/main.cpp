#include <Engine.h>

#include "World/Block.h"
#include "World/Chunk.h"
#include "World/World.h"

#include "Layers/Crosshair.h"
#include "Layers/RenderSettings.h"

#include "Entities/Player.h"

#include <filesystem>

int main(int argc, char const* argv[])
{
    // Output the current working directory using modern c++
    std::filesystem::path path = std::filesystem::current_path();
    std::cout << "Current working directory: " << path << std::endl;

    Engine engine;

    // Starting world thread and configuring
    World world("world", 34985, engine.Entities(), engine.GetRenderer());
    world.Initialize();

    // Create player entity and add it to the entity handler
    // Entity handler is now on the world thread
    Player player(&world, engine.Camera());
    player.SetRenderDistance(15);
    world.SetMainPlayer(&player);
    engine.Entities().AddToUpdater(&player);
    engine.Entities().AddToFrameUpdater(&player);

    // Create the debugging menu
    Settings settings(&world, &engine.GetRenderer(), &engine.Camera());
    Crosshair crosshair;
    engine.Layers().AddLayer(&settings);
    engine.Layers().AddLayer(&crosshair);

    // Starting main engine thread
    engine.Initialize();

    // Ending threads
    world.Finalize();
    engine.Entities().RemoveFromFrameUpdater(&player);
    engine.Entities().RemoveFromUpdater(&player);
    engine.Layers().RemoveLayer(&crosshair);
    engine.Layers().RemoveLayer(&settings);

    return 0;
}
