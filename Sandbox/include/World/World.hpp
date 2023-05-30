#pragma once

#include <Renderer/Renderer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Texture.hpp>
#include <Engine.hpp>

#include "BlockLibrary.hpp"
#include "Chunk.hpp"
#include "ChunkGenerator.hpp"
#include "WorldConfig.hpp"

#include <memory>

class Player;

class World
{
public:
    World() = default;
    World(std::string folderName, long seed);
    ~World();

    void Initialize();
    void Update();
    void Finalize();

    void Generate();
    void GenerateMesh(glm::ivec3 index);

    void SetMainPlayer(Player* player);

    Block GetBlock(glm::ivec3 worldPos);
    void SetBlock(glm::ivec3 worldPos, Block block);
    glm::ivec3 GetChunkCoord(glm::ivec3 worldPos);
    glm::ivec3 GetChunkIndex(glm::ivec3 worldPos);

    void SetShader(const Shader& shader) { m_Shader = shader; }
    void SetSeed(int seed) { ChunkGenerator::SetSeed(seed); }
    long Seed() { return m_Seed; }
    void ProcessRequestedChunks(glm::ivec3 centerChunkIndex);

    // Shader Functions
    const Shader& GetShader() { return m_Shader; }

private:
    // This will be a temporary cache of the loaded chunks.
    // Functionallity to check for saved data on disk will eventually be
    // implemented.
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkDataMap;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkRenderMap;

    std::unordered_set<glm::ivec3> m_ChunksToDelete;
    std::unordered_set<glm::ivec3> m_ChunksToGenerate;

    std::unordered_set<glm::ivec3> m_ChunksToRender;
    std::unordered_set<glm::ivec3> m_ChunksToUnrender;

    std::string m_FolderName;

    Shader m_Shader;
    Texture m_Texture;
    long m_Seed = 0;

    bool m_Running;

    std::mutex m_Lock;

    std::thread m_Thread;

    Player* m_MainPlayer;
};
