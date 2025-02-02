#include "World.h"

#include "BlockLibrary.h"
#include "Chunk.h"
#include "ChunkGenerator.h"

#include "Entities/Player.h"

#include <filesystem>
#include <memory>
#include <print>

World::World(std::string folderName, long seed) : m_FolderName(folderName), m_Seed(seed)
{
    std::filesystem::create_directory(folderName);

    ChunkGenerator::Initialize();
    Blocks::Initialize();
    EntityManager::Initialize();

    // Get current working directory
    std::filesystem::path path = std::filesystem::current_path();

    // Get the absolute path of the shader files
    std::string vertexShaderPath = path.string() + "/Resources/Shaders/PositionTextureNormal.vert";
    std::string fragmentShaderPath = path.string() + "/Resources/Shaders/PositionTextureNormal.frag";

    // Creating shader and texture
    Shader blockShader;
    blockShader.Create(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

    Texture texture;
    std::string texturePath = path.string() + "/Resources/Textures/terrain.png";
    texture.Create(texturePath.c_str());

    TextureMap::Configure(texture.Width(), texture.Height(), 16);

    SetSeed(seed);
    SetShader(blockShader);

    m_ChunkDataMap.clear();
    m_ChunkRenderMap.clear();
    m_ChunksToDelete.clear();
    m_ChunksToGenerate.clear();
    m_ChunksToRender.clear();
    m_ChunksToUnrender.clear();
}

World::~World()
{
}

void World::Initialize()
{
    std::cout << "Initializing world thread...\n";

    m_Running = true;
    m_Thread = std::thread(&World::Update, this);
}

void World::Update()
{
    while (m_Running)
    {
        EntityManager::Update();

        Generate();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::println("Exiting world thread...");
}

void World::Finalize()
{
    m_Running = false;

    std::println("Saving currently loaded chunks...");

    for (auto& chunk : m_ChunkDataMap)
    {
        chunk.second->~Chunk();
    }

    m_Thread.join();
}

Block World::GetBlock(glm::ivec3 worldCoord)
{
    glm::ivec3 index = GetChunkIndex(worldCoord);
    glm::ivec3 chunkCoord = GetChunkCoord(worldCoord);

    if (auto entry = m_ChunkDataMap.find(index); entry != m_ChunkDataMap.end())
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

    if (auto entry = m_ChunkDataMap.find(index); entry != m_ChunkDataMap.end())
    {
        entry->second->SetBlock({chunkCoord.x, chunkCoord.y, chunkCoord.z}, blockToSet);
        entry->second->GetChunkProperties().Modified = true;
        entry->second->GenerateMesh();

        Renderer::UpdateMeshInQueue(index);

        // Updating neighboring meshes in the world
        if (chunkCoord.x == 0)
        {
            m_ChunkDataMap.at({index.x - 1, index.y, index.z})->GenerateMesh();
            Renderer::UpdateMeshInQueue({index.x - 1, index.y, index.z});
        }
        if (chunkCoord.x == 15)
        {
            m_ChunkDataMap.at({index.x + 1, index.y, index.z})->GenerateMesh();
            Renderer::UpdateMeshInQueue({index.x + 1, index.y, index.z});
        }
        if (chunkCoord.z == 0)
        {
            m_ChunkDataMap.at({index.x, index.y, index.z - 1})->GenerateMesh();
            Renderer::UpdateMeshInQueue({index.x, index.y, index.z - 1});
        }
        if (chunkCoord.z == 15)
        {
            m_ChunkDataMap.at({index.x, index.y, index.z + 1})->GenerateMesh();
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
    int renderDistance = m_MainPlayer->GetRenderDistance();

    int lowerx = -chunksToRenderAhead + centerChunkIndex.x - renderDistance;
    int lowerz = -chunksToRenderAhead + centerChunkIndex.z - renderDistance;
    int upperx = 1 + chunksToRenderAhead + centerChunkIndex.x + renderDistance;
    int upperz = 1 + chunksToRenderAhead + centerChunkIndex.z + renderDistance;

    // Add chunks to generation
    for (int x = lowerx; x < upperx; x++)
    {
        for (int z = lowerz; z < upperz; z++)
        {
            index = {x, 0, z};
            chunksGenerated.insert(index);
            if (m_ChunkDataMap.find(index) == m_ChunkDataMap.end())
            {
                m_ChunksToGenerate.insert(index);
            }
        }
    }

    // Removes chunk data
    for (const auto& [index, chunk] : m_ChunkDataMap)
    {
        if (chunksGenerated.find(index) == chunksGenerated.end())
        {
            m_ChunksToDelete.insert(index);
        }
    }

    // Add chunks to render
    for (int x = lowerx + 1; x < upperx - 1; x++)
    {
        for (int z = lowerz + 1; z < upperz - 1; z++)
        {
            index = {x, 0, z};
            chunksRendered.insert(index);
            if (m_ChunkRenderMap.find(index) == m_ChunkRenderMap.end())
            {
                m_ChunksToRender.insert(index);
            }
        }
    }

    // Removes chunk geometry
    for (const auto& [index, chunk] : m_ChunkRenderMap)
    {
        if (chunksRendered.find(index) == chunksRendered.end())
        {
            m_ChunksToUnrender.insert(index);
        }
    }
}

void World::Generate()
{
    // Generates chunks
    for (const auto& index : m_ChunksToGenerate)
    {
        // Generates chunk data
        m_ChunkDataMap[index] = std::make_shared<Chunk>(this, index);
        m_ChunkDataMap.at(index)->Allocate();
        m_ChunkDataMap.at(index)->Generate();
    }
    m_ChunksToGenerate.clear();

    // Renders chunk
    for (const auto& index : m_ChunksToRender)
    {
        // Generate Chunk Geometry
        if (m_ChunkDataMap.find(index) == m_ChunkDataMap.end())
            return;

        std::shared_ptr<Chunk> chunk = m_ChunkDataMap.at(index);
        chunk->GenerateMesh();
        m_ChunkRenderMap.insert_or_assign(index, chunk);

        GenerateMesh(index);
    }
    m_ChunksToRender.clear();

    // Deletes chunks
    for (const auto& index : m_ChunksToDelete)
    {
        // Remove chunk from data
        m_ChunkDataMap.erase(index);
    }
    m_ChunksToDelete.clear();

    // Unrenders chunk
    for (const auto& index : m_ChunksToUnrender)
    {
        // remove mesh from renderer
        Renderer::DeleteMeshFromQueue(index);

        m_ChunkRenderMap.erase(index);
    }
    m_ChunksToUnrender.clear();
}

void World::GenerateMesh(glm::ivec3 index)
{
    std::shared_ptr<Chunk> chunk = m_ChunkDataMap.at(index);
    Mesh mesh(&chunk->GetGeometry()->Vertices, &chunk->GetGeometry()->Indices, &m_Shader);
    Renderer::AddMeshToQueue(index, mesh);
}

void World::SetMainPlayer(Player* player)
{
    m_MainPlayer = player;
}
