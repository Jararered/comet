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
#include <thread>
#include <vector>

class Player;
class EntityManager;

class World
{
public:
    World(std::string folderName, long seed, EntityManager& entityManager, Renderer& renderer);
    ~World();

    void Initialize();
    void Update();
    void Finalize();

    void Generate();
    void GenerateMesh(glm::ivec3 index, bool prioritize = false);

    void SetMainPlayer(Player* player);
    Player* GetMainPlayer() const { return m_MainPlayer; }

    Block GetBlock(glm::ivec3 worldPos);
    void SetBlock(glm::ivec3 worldPos, Block block);
    void SetBlockOverlay(glm::ivec3 worldPos);
    void ClearBlockOverlay();
    glm::ivec3 GetChunkCoord(glm::ivec3 worldPos);
    glm::ivec3 GetChunkIndex(glm::ivec3 worldPos);

    void SetShader(const GameShader& shader) { m_Shader = shader; }
    void SetSeed(int seed) { ChunkGenerator::SetSeed(seed); }
    long Seed() { return m_Seed; }
    void ProcessRequestedChunks(glm::ivec3 centerChunkIndex);

    const GameShader& GetShader() { return m_Shader; }

private:
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkDataMap;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkRenderMap;

    std::unordered_set<glm::ivec3> m_ChunksToDelete;
    std::vector<glm::ivec3> m_ChunksToGenerate;

    std::vector<glm::ivec3> m_ChunksToRender;
    std::vector<glm::ivec3> m_ChunksToRerender;
    std::unordered_set<glm::ivec3> m_ChunksToUnrender;

    std::string m_FolderName;

    GameShader m_Shader;
    GameTexture m_Texture;
    long m_Seed = 0;

    std::atomic_bool m_Running = false;

    std::recursive_mutex m_Lock;

    std::thread m_Thread;

    EntityManager& m_EntityManager;
    Renderer& m_Renderer;
    Player* m_MainPlayer = nullptr;
};
