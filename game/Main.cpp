#include "Engine.h"

#include "interfaces/CrosshairInterface.h"
#include "interfaces/RenderInterface.h"

#include "entities/Player.h"

#include "world/Block.h"
#include "world/Chunk.h"
#include "world/World.h"

int main(void)
{
    // Initializing engine components.
    // This is needed to initialize keyboard/mouse controls,
    // as well as the core OpenGL context.
    Engine::Initialize();

    // Starting world thread and configuring
    World::Initialize();
    World::InitializeThread();

    // Create player entity and add it to the entity handler
    // Entity handler is now on the world thread
    Player player;

    // Create the debugging menu
    RenderInterface renderInterface;
    CrosshairInterface crosshairInterface;

    // Starting main engine thread
    Engine::Thread();

    // Ending threads
    World::Finalize();
    Engine::Finalize();

    return 0;
}
