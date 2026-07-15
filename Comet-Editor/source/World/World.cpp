#include "World.h"

#include "ResourcePaths.h"
#include "BlockLibrary.h"
#include "Chunk.h"
#include "ChunkGenerator.h"

#include "Entities/Player.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <memory>
#include <print>

namespace
{
constexpr int MaxChunkGenerationsPerWorldTick = 1;
constexpr int MaxChunkMeshBuildsPerWorldTick = 1;
constexpr int MaxChunkDeletesPerWorldTick = 4;
constexpr int MaxChunkUnrendersPerWorldTick = 4;
constexpr auto WorldUpdateInterval = std::chrono::milliseconds(25);
}

World::World(std::string folderName, long seed, EntityManager& entityManager, Renderer& renderer)
    : m_FolderName(folderName), m_Seed(seed), m_EntityManager(entityManager), m_Renderer(renderer)
{
    std::filesystem::create_directory(folderName);

    ChunkGenerator::Initialize();
    Blocks::Initialize();

    const std::filesystem::path res = EditorResourcesRoot();

    std::string vertexShaderPath = (res / "Shaders" / "PositionTextureNormal.vert").string();
    std::string fragmentShaderPath = (res / "Shaders" / "PositionTextureNormal.frag").string();

    m_Shader.Create(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

    std::string texturePath = (res / "Textures" / "terrain.png").string();
    m_Texture.Create(texturePath.c_str());

    TextureMap::Configure(m_Texture.Width(), m_Texture.Height(), 16);

    ::Material mat = LoadMaterialDefault();
    mat.shader = m_Shader.GetID();
    SetMaterialTexture(&mat, MATERIAL_MAP_DIFFUSE, m_Texture.GetTexture());
    m_Renderer.SetBlockMaterial(mat);

    SetSeed(seed);

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
        m_EntityManager.Update();

        Generate();

        std::this_thread::sleep_for(WorldUpdateInterval);
    }

    std::println("Exiting world thread...");
}

void World::Finalize()
{
    m_Running = false;

    if (m_Thread.joinable())
    {
        m_Thread.join();
    }

    std::println("Saving currently loaded chunks...");

    std::lock_guard lock(m_Lock);
    m_ChunkRenderMap.clear();
    m_ChunkDataMap.clear();
    m_ChunksToDelete.clear();
    m_ChunksToGenerate.clear();
    m_ChunksToRender.clear();
    m_ChunksToUnrender.clear();
}

Block World::GetBlock(glm::ivec3 worldCoord)
{
    std::lock_guard lock(m_Lock);

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
    if (worldCoord.y < 0 || worldCoord.y >= CHUNK_HEIGHT)
    {
        return;
    }

    std::lock_guard lock(m_Lock);

    glm::ivec3 index = GetChunkIndex(worldCoord);
    glm::ivec3 chunkCoord = GetChunkCoord(worldCoord);

    auto chunkDistanceSquared = [this](const glm::ivec3& chunkIndex)
    {
        if (!m_MainPlayer)
            return 0;

        const glm::ivec3 playerChunk = m_MainPlayer->ChunkIndex();
        const int dx = chunkIndex.x - playerChunk.x;
        const int dz = chunkIndex.z - playerChunk.z;
        return dx * dx + dz * dz;
    };

    auto regenerateChunk = [this](const glm::ivec3& chunkIndex)
    {
        if (auto chunk = m_ChunkDataMap.find(chunkIndex); chunk != m_ChunkDataMap.end())
        {
            chunk->second->GenerateMesh();
            GenerateMesh(chunkIndex, true);
        }
    };

    if (auto entry = m_ChunkDataMap.find(index); entry != m_ChunkDataMap.end())
    {
        entry->second->SetBlock({chunkCoord.x, chunkCoord.y, chunkCoord.z}, blockToSet);
        entry->second->GetChunkProperties().Modified = true;

        std::vector<glm::ivec3> chunksToRegenerate;
        chunksToRegenerate.push_back(index);

        if (chunkCoord.x == 0)
        {
            chunksToRegenerate.push_back({index.x - 1, index.y, index.z});
        }
        if (chunkCoord.x == 15)
        {
            chunksToRegenerate.push_back({index.x + 1, index.y, index.z});
        }
        if (chunkCoord.z == 0)
        {
            chunksToRegenerate.push_back({index.x, index.y, index.z - 1});
        }
        if (chunkCoord.z == 15)
        {
            chunksToRegenerate.push_back({index.x, index.y, index.z + 1});
        }

        std::sort(chunksToRegenerate.begin(), chunksToRegenerate.end(),
            [&chunkDistanceSquared](const glm::ivec3& lhs, const glm::ivec3& rhs)
            {
                const int lhsDistance = chunkDistanceSquared(lhs);
                const int rhsDistance = chunkDistanceSquared(rhs);
                if (lhsDistance != rhsDistance)
                    return lhsDistance < rhsDistance;
                if (lhs.x != rhs.x)
                    return lhs.x < rhs.x;
                return lhs.z < rhs.z;
            });

        for (const glm::ivec3& chunkIndex : chunksToRegenerate)
        {
            regenerateChunk(chunkIndex);
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
    std::lock_guard lock(m_Lock);

    glm::ivec3 index;
    std::unordered_set<glm::ivec3> chunksGenerated;
    std::unordered_set<glm::ivec3> chunksRendered;
    int chunksToRenderAhead = 1;
    int renderDistance = m_MainPlayer->GetRenderDistance();
    int generateRadius = renderDistance + chunksToRenderAhead;

    int lowerx = centerChunkIndex.x - generateRadius;
    int lowerz = centerChunkIndex.z - generateRadius;
    int upperx = 1 + centerChunkIndex.x + generateRadius;
    int upperz = 1 + centerChunkIndex.z + generateRadius;

    auto chunkDistanceSquared = [centerChunkIndex](const glm::ivec3& chunkIndex)
    {
        const int dx = chunkIndex.x - centerChunkIndex.x;
        const int dz = chunkIndex.z - centerChunkIndex.z;
        return dx * dx + dz * dz;
    };

    auto isChunkWithinRadius = [&chunkDistanceSquared](const glm::ivec3& chunkIndex, int radius)
    {
        return chunkDistanceSquared(chunkIndex) <= radius * radius;
    };

    auto sortClosestFirst = [&chunkDistanceSquared](std::vector<glm::ivec3>& chunks)
    {
        std::sort(chunks.begin(), chunks.end(),
            [&chunkDistanceSquared](const glm::ivec3& lhs, const glm::ivec3& rhs)
            {
                const int lhsDistance = chunkDistanceSquared(lhs);
                const int rhsDistance = chunkDistanceSquared(rhs);
                if (lhsDistance != rhsDistance)
                    return lhsDistance < rhsDistance;
                if (lhs.x != rhs.x)
                    return lhs.x < rhs.x;
                return lhs.z < rhs.z;
            });
    };

    m_ChunksToGenerate.clear();
    m_ChunksToRender.clear();

    for (int x = lowerx; x < upperx; x++)
    {
        for (int z = lowerz; z < upperz; z++)
        {
            index = {x, 0, z};
            if (!isChunkWithinRadius(index, generateRadius))
            {
                continue;
            }

            chunksGenerated.insert(index);
            m_ChunksToDelete.erase(index);
            if (m_ChunkDataMap.find(index) == m_ChunkDataMap.end())
            {
                m_ChunksToGenerate.push_back(index);
            }
        }
    }
    sortClosestFirst(m_ChunksToGenerate);

    for (const auto& [index, chunk] : m_ChunkDataMap)
    {
        if (chunksGenerated.find(index) == chunksGenerated.end())
        {
            m_ChunksToDelete.insert(index);
        }
    }

    for (int x = lowerx + 1; x < upperx - 1; x++)
    {
        for (int z = lowerz + 1; z < upperz - 1; z++)
        {
            index = {x, 0, z};
            if (!isChunkWithinRadius(index, renderDistance))
            {
                continue;
            }

            chunksRendered.insert(index);
            m_ChunksToUnrender.erase(index);
            if (m_ChunkRenderMap.find(index) == m_ChunkRenderMap.end())
            {
                m_ChunksToRender.push_back(index);
            }
        }
    }
    sortClosestFirst(m_ChunksToRender);

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
    std::lock_guard lock(m_Lock);

    int chunksGenerated = 0;
    while (!m_ChunksToGenerate.empty() && chunksGenerated < MaxChunkGenerationsPerWorldTick)
    {
        const glm::ivec3 index = m_ChunksToGenerate.front();
        m_ChunksToGenerate.erase(m_ChunksToGenerate.begin());

        if (m_ChunkDataMap.find(index) != m_ChunkDataMap.end())
            continue;

        m_ChunkDataMap[index] = std::make_shared<Chunk>(this, index);
        m_ChunkDataMap.at(index)->Allocate();
        m_ChunkDataMap.at(index)->Generate();

        chunksGenerated++;
    }

    int chunkMeshesBuilt = 0;
    while (!m_ChunksToRender.empty() && chunkMeshesBuilt < MaxChunkMeshBuildsPerWorldTick)
    {
        const glm::ivec3 index = m_ChunksToRender.front();
        m_ChunksToRender.erase(m_ChunksToRender.begin());

        if (m_ChunkDataMap.find(index) == m_ChunkDataMap.end())
            continue;
        if (m_ChunkRenderMap.find(index) != m_ChunkRenderMap.end())
            continue;

        std::shared_ptr<Chunk> chunk = m_ChunkDataMap.at(index);
        chunk->GenerateMesh();
        m_ChunkRenderMap.insert_or_assign(index, chunk);

        GenerateMesh(index);
        chunkMeshesBuilt++;
    }

    int chunksDeleted = 0;
    for (auto deleteIt = m_ChunksToDelete.begin(); deleteIt != m_ChunksToDelete.end() && chunksDeleted < MaxChunkDeletesPerWorldTick;)
    {
        const glm::ivec3 index = *deleteIt;
        m_ChunkDataMap.erase(index);
        deleteIt = m_ChunksToDelete.erase(deleteIt);
        chunksDeleted++;
    }

    int chunksUnrendered = 0;
    for (auto unrenderIt = m_ChunksToUnrender.begin(); unrenderIt != m_ChunksToUnrender.end() && chunksUnrendered < MaxChunkUnrendersPerWorldTick;)
    {
        const glm::ivec3 index = *unrenderIt;
        m_Renderer.DeleteMeshFromQueue(index);

        m_ChunkRenderMap.erase(index);
        unrenderIt = m_ChunksToUnrender.erase(unrenderIt);
        chunksUnrendered++;
    }
}

void World::GenerateMesh(glm::ivec3 index, bool prioritize)
{
    std::lock_guard lock(m_Lock);

    std::shared_ptr<Chunk> chunk = m_ChunkDataMap.at(index);
    GameMesh mesh(&chunk->GetGeometry()->Vertices, &chunk->GetGeometry()->Indices, &m_Shader);
    GameMesh waterMesh(&chunk->GetWaterGeometry()->Vertices, &chunk->GetWaterGeometry()->Indices, &m_Shader);
    if (prioritize)
        m_Renderer.AddPriorityMeshToQueue(index, mesh);
    else
        m_Renderer.AddMeshToQueue(index, mesh);
    m_Renderer.AddWaterMeshToQueue(index, waterMesh);
}

void World::SetMainPlayer(Player* player)
{
    m_MainPlayer = player;
}
