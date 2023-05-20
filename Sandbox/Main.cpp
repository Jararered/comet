#include <Comet/Engine.hpp>

#include "World/Block.hpp"
#include "World/Chunk.hpp"
#include "World/World.hpp"
#include "Interfaces/CrosshairInterface.hpp"
#include "Interfaces/RenderInterface.hpp"
#include "Entities/Player.hpp"

int main(int argc, char const *argv[])
{
    // Initializing engine components.
    // This is needed to initialize keyboard/mouse controls,
    // as well as the core OpenGL context.
    Engine::Initialize();

    // Create player entity and add it to the entity handler
    // Entity handler is now on the world thread
    Player player;

    // Starting world thread and configuring
    World::Initialize();
    World::InitializeThread();

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
