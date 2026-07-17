#pragma once

#include <Engine.h>
#include <Renderer/Renderer.h>
#include <Renderer/Shader.h>
#include <Renderer/Texture.h>

#include "BlockLibrary.h"
#include "Chunk.h"
#include "ChunkGenerator.h"
#include "WorldConfig.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

#include <glm/mat4x4.hpp>

class Player;
class EntityManager;

class World
{
public:
    struct StreamingSnapshot
    {
        glm::ivec3 CenterChunkIndex = {0, 0, 0};
        glm::mat4 ViewProjection = glm::mat4(1.0f);
        int RenderDistance = 0;
        bool Valid = false;
    };

    World(std::string folderName, long seed, EntityManager& entityManager, Renderer& renderer);
    ~World();

    void Initialize();
    void Update();
    void Finalize();

    void Generate();
    void GenerateMesh(glm::ivec3 index, bool prioritize = false);

    Player& AddPlayer();
    void SetMainPlayer(Player* player);
    Player* GetMainPlayer() const { return m_MainPlayer; }

    Block GetBlock(glm::ivec3 worldPos);
    // Caller must hold m_Lock in shared or exclusive mode for the duration of the lookup.
    Block GetBlockForMeshing(glm::ivec3 worldPos);
    void SetBlock(glm::ivec3 worldPos, Block block);
    void SetBlockOverlay(glm::ivec3 worldPos);
    void ClearBlockOverlay();
    glm::ivec3 GetChunkCoord(glm::ivec3 worldPos);
    glm::ivec3 GetChunkIndex(glm::ivec3 worldPos);

    void SetShader(const GameShader& shader) { m_Shader = shader; }
    void SetSeed(int seed) { ChunkGenerator::SetSeed(seed); }
    long Seed() { return m_Seed; }
    void PublishStreamingSnapshot(glm::ivec3 centerChunkIndex, const glm::mat4& viewProjection, int renderDistance);

    const GameShader& GetShader() { return m_Shader; }

private:
    void ProcessRequestedChunks(const StreamingSnapshot& snapshot);

    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkDataMap;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkRenderMap;

    std::unordered_set<glm::ivec3> m_ChunksToDelete;
    std::vector<glm::ivec3> m_ChunksToGenerate;

    std::vector<glm::ivec3> m_ChunksToRender;
    // Player edits must not wait for the background streaming remesh queue.
    std::vector<glm::ivec3> m_ChunksToPriorityRerender;
    std::vector<glm::ivec3> m_ChunksToRerender;
    std::unordered_set<glm::ivec3> m_ChunksToUnrender;

    std::string m_FolderName;

    GameShader m_Shader;
    GameTexture m_Texture;
    long m_Seed = 0;

    std::atomic_bool m_Running = false;
    bool m_Finalized = false;

    std::shared_mutex m_Lock;
    std::mutex m_StreamingSnapshotMutex;
    StreamingSnapshot m_StreamingSnapshot;

    std::thread m_Thread;

    EntityManager& m_EntityManager;
    Renderer& m_Renderer;
    std::vector<std::unique_ptr<Player>> m_Players;
    Player* m_MainPlayer = nullptr;
};
