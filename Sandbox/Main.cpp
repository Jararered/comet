#include <Engine.h>

#include "World/Block.h"
#include "World/Chunk.h"
#include "World/World.h"

#include "Layers/Crosshair.h"
#include "Layers/RenderSettings.h"

#include "Entities/Player.h"

int main(int argc, char const* argv[])
{
    Engine* engine = new Engine();

    // Starting world thread and configuring
    World* world = new World("world", 34985);
    world->Initialize();

    // Create player entity and add it to the entity handler
    // Entity handler is now on the world thread
    Player* player = new Player(world);
    player->SetRenderDistance(8);
    world->SetMainPlayer(player);
    EntityManager::AddToUpdater(player);
    EntityManager::AddToFrameUpdater(player);

    // Create the debugging menu
    Settings* settings = new Settings;
    Crosshair* crosshair = new Crosshair;
    LayerManager::AddLayer(settings);
    LayerManager::AddLayer(crosshair);


    // Starting main engine thread
    engine->Initialize();

    // Ending threads
    world->Finalize();
    
    return 0;
}
