#include "World.h"

#include "BlockLibrary.h"
#include "Chunk.h"
#include "ChunkGenerator.h"
#include "ResourcePaths.h"
#include <Profiler/Profiler.h>

#include "Entities/Player.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <memory>
#include <print>

namespace
{
    constexpr int MaxChunkGenerationsPerWorldTick = 1;
    constexpr int MaxChunkMeshBuildsPerWorldTick = 1;
    constexpr int MaxChunkRemeshesPerWorldTick = 1;
    constexpr int MaxChunkDeletesPerWorldTick = 4;
    constexpr int MaxChunkUnrendersPerWorldTick = 4;
    constexpr auto WorldUpdateInterval = std::chrono::milliseconds(25);

    struct Frustum
    {
        std::array<glm::vec4, 6> Planes;
    };

    glm::vec4 MatrixRow(const glm::mat4& matrix, int row)
    {
        return {matrix[0][row], matrix[1][row], matrix[2][row], matrix[3][row]};
    }

    glm::vec4 NormalizePlane(const glm::vec4& plane)
    {
        const float length = glm::length(glm::vec3(plane));
        if (length <= 0.0f)
        {
            return plane;
        }
        return plane / length;
    }

    Frustum BuildFrustumFromViewProjection(const glm::mat4& viewProjection)
    {
        const glm::vec4 row0 = MatrixRow(viewProjection, 0);
        const glm::vec4 row1 = MatrixRow(viewProjection, 1);
        const glm::vec4 row2 = MatrixRow(viewProjection, 2);
        const glm::vec4 row3 = MatrixRow(viewProjection, 3);

        return {{NormalizePlane(row3 + row0), NormalizePlane(row3 - row0), NormalizePlane(row3 + row1), NormalizePlane(row3 - row1), NormalizePlane(row3 + row2), NormalizePlane(row3 - row2)}};
    }

    bool IsChunkColumnVisible(const Frustum& frustum, const glm::ivec3& chunkIndex)
    {
        const glm::vec3 min = {static_cast<float>(chunkIndex.x * CHUNK_WIDTH) - 0.5f, -0.5f, static_cast<float>(chunkIndex.z * CHUNK_WIDTH) - 0.5f};
        const glm::vec3 max = {static_cast<float>((chunkIndex.x + 1) * CHUNK_WIDTH) - 0.5f, static_cast<float>(CHUNK_HEIGHT) - 0.5f, static_cast<float>((chunkIndex.z + 1) * CHUNK_WIDTH) - 0.5f};

        for (const glm::vec4& plane : frustum.Planes)
        {
            const glm::vec3 positiveVertex = {plane.x >= 0.0f ? max.x : min.x, plane.y >= 0.0f ? max.y : min.y, plane.z >= 0.0f ? max.z : min.z};

            if (glm::dot(glm::vec3(plane), positiveVertex) + plane.w < 0.0f)
            {
                return false;
            }
        }

        return true;
    }

    void PushUnique(std::vector<glm::ivec3>& chunks, const glm::ivec3& chunk)
    {
        if (std::find(chunks.begin(), chunks.end(), chunk) == chunks.end())
        {
            chunks.push_back(chunk);
        }
    }
}

World::World(std::string folderName, long seed, EntityManager& entityManager, Renderer& renderer) : m_FolderName(folderName), m_Seed(seed), m_EntityManager(entityManager), m_Renderer(renderer)
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
    Finalize();
}

void World::Initialize()
{
    std::cout << "Initializing world thread...\n";

    m_Finalized = false;
    m_Running = true;
    m_Thread = std::thread(&World::Update, this);
}

void World::Update()
{
    while (m_Running)
    {
        try
        {
            COMET_PROFILE_SCOPE("World::Tick", "world");

            StreamingSnapshot snapshot;
            {
                std::lock_guard lock(m_StreamingSnapshotMutex);
                snapshot = m_StreamingSnapshot;
            }
            ProcessRequestedChunks(snapshot);

            m_EntityManager.Update();

            Generate();
        }
        catch (const std::exception& exception)
        {
            std::println(stderr, "FATAL: World thread stopped: {}", exception.what());
            throw;
        }
        catch (...)
        {
            std::println(stderr, "FATAL: World thread stopped by an unknown exception");
            throw;
        }

        std::this_thread::sleep_for(WorldUpdateInterval);
        Comet::Profiler::Instance().FlushIfDue();
    }

    Comet::Profiler::Instance().FlushIfDue();
    std::println("Exiting world thread...");
}

void World::Finalize()
{
    if (m_Finalized)
        return;

    m_Running = false;

    if (m_Thread.joinable())
    {
        m_Thread.join();
    }

    for (const std::unique_ptr<Player>& player : m_Players)
    {
        m_EntityManager.RemoveFromPhysicsUpdater(player.get());
        m_EntityManager.RemoveFromFrameUpdater(player.get());
        m_EntityManager.RemoveFromUpdater(player.get());
    }
    SetMainPlayer(nullptr);
    m_Players.clear();

    std::println("Saving currently loaded chunks...");

    std::lock_guard lock(m_Lock);
    m_ChunkRenderMap.clear();
    m_ChunkDataMap.clear();
    m_ChunksToDelete.clear();
    m_ChunksToGenerate.clear();
    m_ChunksToRender.clear();
    m_ChunksToPriorityRerender.clear();
    m_ChunksToRerender.clear();
    m_ChunksToUnrender.clear();
    m_Finalized = true;
}

Block World::GetBlock(glm::ivec3 worldCoord)
{
    std::shared_lock lock(m_Lock);

    return GetBlockForMeshing(worldCoord);
}

Block World::GetBlockForMeshing(glm::ivec3 worldCoord)
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
            if (m_ChunkRenderMap.find(chunkIndex) != m_ChunkRenderMap.end())
            {
                PushUnique(m_ChunksToPriorityRerender, chunkIndex);
            }
        }
    }
}

void World::SetBlockOverlay(glm::ivec3 worldPos)
{
    m_Renderer.SetBlockOverlay(worldPos);
}

void World::ClearBlockOverlay()
{
    m_Renderer.ClearBlockOverlay();
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

void World::PublishStreamingSnapshot(glm::ivec3 centerChunkIndex, const glm::mat4& viewProjection, int renderDistance)
{
    std::lock_guard lock(m_StreamingSnapshotMutex);
    m_StreamingSnapshot.CenterChunkIndex = centerChunkIndex;
    m_StreamingSnapshot.ViewProjection = viewProjection;
    m_StreamingSnapshot.RenderDistance = renderDistance;
    m_StreamingSnapshot.Valid = true;
}

void World::ProcessRequestedChunks(const StreamingSnapshot& snapshot)
{
    if (!snapshot.Valid)
        return;

    COMET_PROFILE_SCOPE("World::ProcessRequestedChunks", "world");

    const glm::ivec3 centerChunkIndex = snapshot.CenterChunkIndex;
    const int renderDistance = snapshot.RenderDistance;
    constexpr int chunksToRenderAhead = 1;
    const int generateRadius = renderDistance + chunksToRenderAhead;
    const Frustum cameraFrustum = BuildFrustumFromViewProjection(snapshot.ViewProjection);

    const int lowerx = centerChunkIndex.x - generateRadius;
    const int lowerz = centerChunkIndex.z - generateRadius;
    const int upperx = 1 + centerChunkIndex.x + generateRadius;
    const int upperz = 1 + centerChunkIndex.z + generateRadius;

    auto chunkDistanceSquared = [centerChunkIndex](const glm::ivec3& chunkIndex)
    {
        const int dx = chunkIndex.x - centerChunkIndex.x;
        const int dz = chunkIndex.z - centerChunkIndex.z;
        return dx * dx + dz * dz;
    };

    auto isChunkWithinRadius = [&chunkDistanceSquared](const glm::ivec3& chunkIndex, int radius) { return chunkDistanceSquared(chunkIndex) <= radius * radius; };

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

    std::unordered_set<glm::ivec3> dataKeys;
    std::unordered_set<glm::ivec3> renderKeys;
    {
        std::shared_lock lock(m_Lock);
        dataKeys.reserve(m_ChunkDataMap.size());
        for (const auto& [index, chunk] : m_ChunkDataMap)
        {
            dataKeys.insert(index);
        }
        renderKeys.reserve(m_ChunkRenderMap.size());
        for (const auto& [index, chunk] : m_ChunkRenderMap)
        {
            renderKeys.insert(index);
        }
    }

    std::unordered_set<glm::ivec3> chunksGenerated;
    std::unordered_set<glm::ivec3> chunksRendered;
    std::vector<glm::ivec3> chunksToGenerate;
    std::vector<glm::ivec3> chunksToRender;
    std::unordered_set<glm::ivec3> chunksToDelete;
    std::unordered_set<glm::ivec3> chunksToUnrender;

    for (int x = lowerx; x < upperx; x++)
    {
        for (int z = lowerz; z < upperz; z++)
        {
            const glm::ivec3 index = {x, 0, z};
            if (!isChunkWithinRadius(index, generateRadius))
            {
                continue;
            }

            chunksGenerated.insert(index);
            if (dataKeys.find(index) == dataKeys.end() && IsChunkColumnVisible(cameraFrustum, index))
            {
                chunksToGenerate.push_back(index);
            }
        }
    }
    sortClosestFirst(chunksToGenerate);

    for (const glm::ivec3& index : dataKeys)
    {
        if (chunksGenerated.find(index) == chunksGenerated.end())
        {
            chunksToDelete.insert(index);
        }
    }

    for (int x = lowerx + 1; x < upperx - 1; x++)
    {
        for (int z = lowerz + 1; z < upperz - 1; z++)
        {
            const glm::ivec3 index = {x, 0, z};
            if (!isChunkWithinRadius(index, renderDistance))
            {
                continue;
            }

            chunksRendered.insert(index);
            if (renderKeys.find(index) == renderKeys.end() && IsChunkColumnVisible(cameraFrustum, index))
            {
                chunksToRender.push_back(index);
            }
        }
    }
    sortClosestFirst(chunksToRender);

    for (const glm::ivec3& index : renderKeys)
    {
        if (chunksRendered.find(index) == chunksRendered.end())
        {
            chunksToUnrender.insert(index);
        }
    }

    {
        std::lock_guard lock(m_Lock);
        m_ChunksToGenerate = std::move(chunksToGenerate);
        m_ChunksToRender = std::move(chunksToRender);
        m_ChunksToDelete = std::move(chunksToDelete);
        m_ChunksToUnrender = std::move(chunksToUnrender);
    }
}

void World::Generate()
{
    COMET_PROFILE_SCOPE("World::Generate", "world");
    int chunksGenerated = 0;
    while (chunksGenerated < MaxChunkGenerationsPerWorldTick)
    {
        glm::ivec3 index;
        {
            std::unique_lock lock(m_Lock);
            if (m_ChunksToGenerate.empty())
                break;

            index = m_ChunksToGenerate.front();
            m_ChunksToGenerate.erase(m_ChunksToGenerate.begin());
            if (m_ChunkDataMap.find(index) != m_ChunkDataMap.end())
                continue;
        }

        auto chunk = std::make_shared<Chunk>(this, index);
        chunk->Allocate();
        chunk->Generate();

        {
            std::unique_lock lock(m_Lock);
            if (m_ChunkDataMap.find(index) != m_ChunkDataMap.end())
                continue;

            m_ChunkDataMap.insert_or_assign(index, std::move(chunk));
        }

        chunksGenerated++;
    }

    int chunkMeshesBuilt = 0;
    while (chunkMeshesBuilt < MaxChunkMeshBuildsPerWorldTick)
    {
        glm::ivec3 index;
        std::shared_ptr<Chunk> chunk;
        {
            std::unique_lock lock(m_Lock);
            if (m_ChunksToRender.empty())
                break;

            index = m_ChunksToRender.front();
            m_ChunksToRender.erase(m_ChunksToRender.begin());

            if (m_ChunkDataMap.find(index) == m_ChunkDataMap.end() || m_ChunkRenderMap.find(index) != m_ChunkRenderMap.end())
                continue;

            chunk = m_ChunkDataMap.at(index);
        }

        {
            std::shared_lock lock(m_Lock);
            chunk->GenerateMesh();
        }

        {
            std::unique_lock lock(m_Lock);
            m_ChunkRenderMap.insert_or_assign(index, chunk);
        }
        GenerateMesh(index);

        std::unique_lock lock(m_Lock);
        const std::array<glm::ivec3, 4> neighbors = {
            glm::ivec3{index.x - 1, index.y, index.z},
            glm::ivec3{index.x + 1, index.y, index.z},
            glm::ivec3{index.x, index.y, index.z - 1},
            glm::ivec3{index.x, index.y, index.z + 1},
        };
        for (const glm::ivec3& neighbor : neighbors)
        {
            if (m_ChunkRenderMap.find(neighbor) != m_ChunkRenderMap.end())
            {
                PushUnique(m_ChunksToRerender, neighbor);
            }
        }
        chunkMeshesBuilt++;
    }

    auto remeshQueuedChunks = [this](std::vector<glm::ivec3>& queue, int maxRemeshes)
    {
        int chunksRemeshed = 0;
        while (chunksRemeshed < maxRemeshes)
        {
            glm::ivec3 index;
            std::shared_ptr<Chunk> chunk;
            {
                std::unique_lock lock(m_Lock);
                if (queue.empty())
                    break;

                index = queue.front();
                queue.erase(queue.begin());
                if (auto entry = m_ChunkRenderMap.find(index); entry != m_ChunkRenderMap.end())
                    chunk = entry->second;
            }

            if (chunk)
            {
                std::shared_lock lock(m_Lock);
                chunk->GenerateMesh();
                lock.unlock();
                GenerateMesh(index, true);
                chunksRemeshed++;
            }
        }
        return chunksRemeshed;
    };

    // Local block changes take a remesh slot even while chunk streaming is busy.
    // The regular queue remains idle-only to keep neighbor maintenance from adding hitches.
    const int priorityRemeshes = remeshQueuedChunks(m_ChunksToPriorityRerender, MaxChunkRemeshesPerWorldTick);
    if (priorityRemeshes == 0 && chunksGenerated == 0 && chunkMeshesBuilt == 0)
    {
        remeshQueuedChunks(m_ChunksToRerender, MaxChunkRemeshesPerWorldTick);
    }

    {
        std::unique_lock lock(m_Lock);
        int chunksDeleted = 0;
        for (auto deleteIt = m_ChunksToDelete.begin(); deleteIt != m_ChunksToDelete.end() && chunksDeleted < MaxChunkDeletesPerWorldTick;)
        {
            m_ChunkDataMap.erase(*deleteIt);
            deleteIt = m_ChunksToDelete.erase(deleteIt);
            chunksDeleted++;
        }

        int chunksUnrendered = 0;
        for (auto unrenderIt = m_ChunksToUnrender.begin(); unrenderIt != m_ChunksToUnrender.end() && chunksUnrendered < MaxChunkUnrendersPerWorldTick;)
        {
            m_Renderer.DeleteMeshFromQueue(*unrenderIt);
            m_ChunkRenderMap.erase(*unrenderIt);
            std::erase(m_ChunksToPriorityRerender, *unrenderIt);
            std::erase(m_ChunksToRerender, *unrenderIt);
            unrenderIt = m_ChunksToUnrender.erase(unrenderIt);
            chunksUnrendered++;
        }

#ifndef NDEBUG
        static auto nextStreamLog = std::chrono::steady_clock::now();
        const auto now = std::chrono::steady_clock::now();
        if (now >= nextStreamLog)
        {
            std::println("STREAM: data={} rendered={} generate={} render={} remesh={} delete={} unrender={}", m_ChunkDataMap.size(), m_ChunkRenderMap.size(), m_ChunksToGenerate.size(),
                         m_ChunksToRender.size(), m_ChunksToPriorityRerender.size() + m_ChunksToRerender.size(), m_ChunksToDelete.size(), m_ChunksToUnrender.size());
            nextStreamLog = now + std::chrono::seconds(1);
        }

        for (const auto& [index, chunk] : m_ChunkRenderMap)
        {
            assert(chunk != nullptr && "Rendered chunk entry must own a Chunk");
        }
#endif
    }
}

void World::GenerateMesh(glm::ivec3 index, bool prioritize)
{
    std::shared_lock lock(m_Lock);

    std::shared_ptr<Chunk> chunk;
    if (auto entry = m_ChunkDataMap.find(index); entry != m_ChunkDataMap.end())
    {
        chunk = entry->second;
    }
    else if (auto entry = m_ChunkRenderMap.find(index); entry != m_ChunkRenderMap.end())
    {
        // Streaming may evict source data before a previously queued remesh is
        // consumed. The rendered map retains the chunk until its GPU mesh is removed.
        chunk = entry->second;
    }

    if (!chunk)
    {
        std::println(stderr, "STREAM ERROR: tried to queue a mesh for missing chunk ({}, {}, {})", index.x, index.y, index.z);
        assert(false && "Mesh queue entry must have a source or rendered chunk");
        return;
    }

    GameMesh mesh(std::move(chunk->GetGeometry()->Vertices), std::move(chunk->GetGeometry()->Indices), &m_Shader);
    GameMesh waterMesh(std::move(chunk->GetWaterGeometry()->Vertices), std::move(chunk->GetWaterGeometry()->Indices), &m_Shader);
    m_Renderer.AddChunkMeshToQueue(index, std::move(mesh), std::move(waterMesh), prioritize);
}

Player& World::AddPlayer()
{
    std::unique_ptr<Player> player = std::make_unique<Player>(this);
    Player& playerReference = *player;
    m_Players.push_back(std::move(player));

    m_EntityManager.AddToUpdater(&playerReference);
    m_EntityManager.AddToFrameUpdater(&playerReference);
    m_EntityManager.AddToPhysicsUpdater(&playerReference);

    if (m_MainPlayer == nullptr)
        SetMainPlayer(&playerReference);

    return playerReference;
}

void World::SetMainPlayer(Player* player)
{
    if (player != nullptr)
    {
        const bool belongsToWorld = std::ranges::any_of(m_Players, [player](const std::unique_ptr<Player>& ownedPlayer) { return ownedPlayer.get() == player; });
        assert(belongsToWorld && "Main player must belong to this world");
        if (!belongsToWorld)
            return;
    }

    m_MainPlayer = player;
    m_Renderer.SetCamera(player != nullptr ? player->Camera() : std::shared_ptr<Comet::ViewCamera>{});
}
