#include <Engine.h>

#include "World/Block.h"
#include "World/Chunk.h"
#include "World/World.h"

#include "Layers/Crosshair.h"
#include "Layers/RenderSettings.h"

#include "Entities/Player.h"

int main(int argc, char const* argv[])
{
    Engine engine;

    // Starting world thread and configuring
    World world("world", 34985, engine.Entities(), engine.GetRenderer());
    world.Initialize();

    // Create player entity and add it to the entity handler
    // Entity handler is now on the world thread
    Player& player = world.AddPlayer();
    player.SetRenderDistance(10);

    // Create the debugging menu
    engine.Layers().AddLayer<Settings>("settings", &world, &engine.GetRenderer(), player.Camera());
    engine.Layers().AddLayer<Crosshair>("crosshair");

    // Starting main engine thread
    engine.Initialize();

    // Ending threads
    world.Finalize();
    engine.Layers().RemoveLayer("crosshair");
    engine.Layers().RemoveLayer("settings");

    return 0;
}
