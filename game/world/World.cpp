#include "World.h"

#include "world/BlockLibrary.h"
#include "world/Chunk.h"
#include "world/ChunkGenerator.h"

#include "entities/Player.h"
#include <memory>

void World::Initialize()
{
    std::filesystem::create_directory("worlddata");

    ChunkGenerator::Initialize();
    Blocks::Initialize();
    EntityHandler::Initialize();

    // Shader/Texture Setup
    ShaderProgram blockShader("../game/shaders/PositionTextureNormal.vert", "../game/shaders/PositionTextureNormal.frag");
    Texture texture("../game/textures/terrain.png");
    TextureMap::Configure(texture.Width(), texture.Height(), 16);

    World::SetSeed(1);
    World::SetShader(blockShader);
    World::SetRenderDistance(8);

    Instance().m_ChunkDataMap.clear();
    Instance().m_ChunkRenderMap.clear();
    Instance().m_ChunksToDelete.clear();
    Instance().m_ChunksToGenerate.clear();
    Instance().m_ChunksToRender.clear();
    Instance().m_ChunksToUnrender.clear();
}

void World::InitializeThread()
{
    std::cout << "Initializing world thread...\n";

    Instance().m_Thread = std::thread(&World::Thread);
}

void World::Thread()
{
    while (!Engine::IsShouldClose() && !Renderer::IsResetting())
    {
        EntityHandler::Update();

        Generate();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Exiting world thread...\n";
}

void World::Finalize()
{
    std::cout << "Saving currently loaded chunks...\n";

    for (auto &chunk : Instance().m_ChunkDataMap)
    {
        chunk.second->~Chunk();
    }

    Instance().m_Thread.join();
}

Block World::GetBlock(glm::ivec3 worldCoord)
{
    glm::ivec3 index = GetChunkIndex(worldCoord);
    glm::ivec3 chunkCoord = GetChunkCoord(worldCoord);

    if (auto entry = Instance().m_ChunkDataMap.find(index); entry != Instance().m_ChunkDataMap.end())
    {
        return entry->second->GetBlock(chunkCoord);
    }
    else
    {
        return Blocks::Air();
    }
}

void World::SetBlock(glm::ivec3 worldCoord, Block blockToSet)
{
    if (worldCoord.y > CHUNK_HEIGHT)
    {
        return;
    }

    glm::ivec3 index = GetChunkIndex(worldCoord);
    glm::ivec3 chunkCoord = GetChunkCoord(worldCoord);

    if (auto entry = Instance().m_ChunkDataMap.find(index); entry != Instance().m_ChunkDataMap.end())
    {
        entry->second->SetBlock({chunkCoord.x, chunkCoord.y, chunkCoord.z}, blockToSet);
        entry->second->SetModified(true);
        entry->second->GenerateMesh();

        Renderer::UpdateMeshInQueue(index);

        // Updating neighboring meshes in the world
        if (chunkCoord.x == 0)
        {
            Instance().m_ChunkDataMap.at({index.x - 1, index.y, index.z})->GenerateMesh();
            Renderer::UpdateMeshInQueue({index.x - 1, index.y, index.z});
        }
        if (chunkCoord.x == 15)
        {
            Instance().m_ChunkDataMap.at({index.x + 1, index.y, index.z})->GenerateMesh();
            Renderer::UpdateMeshInQueue({index.x + 1, index.y, index.z});
        }
        if (chunkCoord.z == 0)
        {
            Instance().m_ChunkDataMap.at({index.x, index.y, index.z - 1})->GenerateMesh();
            Renderer::UpdateMeshInQueue({index.x, index.y, index.z - 1});
        }
        if (chunkCoord.z == 15)
        {
            Instance().m_ChunkDataMap.at({index.x, index.y, index.z + 1})->GenerateMesh();
            Renderer::UpdateMeshInQueue({index.x, index.y, index.z + 1});
        }
    }
}

glm::ivec3 World::GetChunkCoord(glm::ivec3 worldCoord)
{
    while (worldCoord.x < 0)
    {
        worldCoord.x += CHUNK_WIDTH;
    }
    while (worldCoord.x > CHUNK_WIDTH - 1)
    {
        worldCoord.x -= CHUNK_WIDTH;
    }
    while (worldCoord.z < 0)
    {
        worldCoord.z += CHUNK_WIDTH;
    }
    while (worldCoord.z > CHUNK_WIDTH - 1)
    {
        worldCoord.z -= CHUNK_WIDTH;
    }

    return worldCoord;
}

glm::ivec3 World::GetChunkIndex(glm::ivec3 worldCoord)
{
    glm::ivec3 chunkIndex(0, 0, 0);

    chunkIndex.x = std::floor(static_cast<double>(worldCoord.x) / static_cast<double>(CHUNK_WIDTH));
    chunkIndex.z = std::floor(static_cast<double>(worldCoord.z) / static_cast<double>(CHUNK_WIDTH));

    return chunkIndex;
}

void World::ProcessRequestedChunks(glm::ivec3 centerChunkIndex)
{
    glm::ivec3 index;
    std::unordered_set<glm::ivec3> chunksGenerated;
    std::unordered_set<glm::ivec3> chunksRendered;
    int chunksToRenderAhead = 1;

    int lowerx = -chunksToRenderAhead + centerChunkIndex.x - Instance().m_RenderDistance;
    int lowerz = -chunksToRenderAhead + centerChunkIndex.z - Instance().m_RenderDistance;
    int upperx = 1 + chunksToRenderAhead + centerChunkIndex.x + Instance().m_RenderDistance;
    int upperz = 1 + chunksToRenderAhead + centerChunkIndex.z + Instance().m_RenderDistance;

    // Add chunks to generation
    for (int x = lowerx; x < upperx; x++)
    {
        for (int z = lowerz; z < upperz; z++)
        {
            if (Instance().m_RenderShape == RenderShape::Circle)
            {
                double distSqrd = ((x - centerChunkIndex.x) * (x - centerChunkIndex.x)) +
                                  ((z - centerChunkIndex.z) * (z - centerChunkIndex.z));
                double renderDistSqrd = (Instance().m_RenderDistance + 1) * (Instance().m_RenderDistance + 1);

                if (distSqrd > renderDistSqrd)
                {
                    continue;
                }
            }

            index = {x, 0, z};
            chunksGenerated.insert(index);
            if (Instance().m_ChunkDataMap.find(index) == Instance().m_ChunkDataMap.end())
            {
                Instance().m_ChunksToGenerate.insert(index);
            }
        }
    }

    // Removes chunk data
    for (const auto &[index, chunk] : Instance().m_ChunkDataMap)
    {
        if (chunksGenerated.find(index) == chunksGenerated.end())
        {
            Instance().m_ChunksToDelete.insert(index);
        }
    }

    // Add chunks to render
    for (int x = lowerx + 1; x < upperx - 1; x++)
    {
        for (int z = lowerz + 1; z < upperz - 1; z++)
        {

            if (Instance().m_RenderShape == RenderShape::Circle)
            {
                double distSqrd = ((x - centerChunkIndex.x) * (x - centerChunkIndex.x)) +
                                  ((z - centerChunkIndex.z) * (z - centerChunkIndex.z));

                // subtracting 0.1 to smooth out the circle a little bit
                double renderDistSqrd = Instance().m_RenderDistance * Instance().m_RenderDistance - 0.1;

                if (distSqrd > renderDistSqrd)
                {
                    continue;
                }
            }

            index = {x, 0, z};
            chunksRendered.insert(index);
            if (Instance().m_ChunkRenderMap.find(index) == Instance().m_ChunkRenderMap.end())
            {
                Instance().m_ChunksToRender.insert(index);
            }
        }
    }

    // Removes chunk geometry
    for (const auto &[index, chunk] : Instance().m_ChunkRenderMap)
    {
        if (chunksRendered.find(index) == chunksRendered.end())
        {
            Instance().m_ChunksToUnrender.insert(index);
        }
    }
}

void World::Generate()
{
    // Generates chunks
    for (const auto &index : Instance().m_ChunksToGenerate)
    {
        // Generates chunk data
        Instance().m_ChunkDataMap[index] = std::make_shared<Chunk>(index);
        Instance().m_ChunkDataMap.at(index)->Allocate();
        Instance().m_ChunkDataMap.at(index)->Generate();
    }
    Instance().m_ChunksToGenerate.clear();

    // Renders chunk
    for (const auto &index : Instance().m_ChunksToRender)
    {
        // Generate Chunk Geometry
        if (Instance().m_ChunkDataMap.find(index) == Instance().m_ChunkDataMap.end())
            return;

        std::shared_ptr<Chunk> chunk = Instance().m_ChunkDataMap.at(index);
        chunk->GenerateMesh();
        Instance().m_ChunkRenderMap.insert_or_assign(index, chunk);

        GenerateMesh(index);
    }
    Instance().m_ChunksToRender.clear();

    // Deletes chunks
    for (const auto &index : Instance().m_ChunksToDelete)
    {
        // Remove chunk from data
        Instance().m_ChunkDataMap.erase(index);
    }
    Instance().m_ChunksToDelete.clear();

    // Unrenders chunk
    for (const auto &index : Instance().m_ChunksToUnrender)
    {
        // remove mesh from renderer
        Renderer::DeleteMeshFromQueue(index);

        Instance().m_ChunkRenderMap.erase(index);
    }
    Instance().m_ChunksToUnrender.clear();
}

void World::GenerateMesh(glm::ivec3 index)
{
    std::shared_ptr<Chunk> chunk = Instance().m_ChunkDataMap.at(index);
    Mesh mesh(&chunk->Geometry()->Vertices, &chunk->Geometry()->Indices, &Instance().m_Shader);
    Renderer::AddMeshToQueue(index, mesh);
}
