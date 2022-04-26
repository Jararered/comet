#pragma once

#include <comet.pch>

#include "render/ShaderProgram.h"

#include "BlockLibrary.h"
#include "Chunk.h"
#include "ChunkGenerator.h"
#include "Engine.h"
#include "Renderer.h"
#include "WorldConfig.h"
#include "render/Texture.h"

class World
{
  public:
    inline static auto &Instance()
    {
        static World instance;
        return instance;
    }
    static void InitializeThread();
    static void Thread();

    static void Initialize();
    static void Finalize();

    static void Generate();
    static void GenerateMesh(glm::ivec3 index, Chunk *chunk);

    static Block GetBlock(glm::ivec3 worldPos);
    static void SetBlock(glm::ivec3 worldPos, Block block);
    static glm::ivec3 GetChunkCoord(glm::ivec3 worldPos);
    static glm::ivec3 GetChunkIndex(glm::ivec3 worldPos);

    static void SetShader(const ShaderProgram &shader) { Instance().m_Shader = shader; }
    static void SetSeed(int seed) { ChunkGenerator::SetSeed(seed); }
    static int Seed() { return Instance().m_Seed; }
    static void ProcessRequestedChunks(glm::ivec3 centerChunkIndex);

    // Shader Functions
    const ShaderProgram &GetShader() { return m_Shader; }

    static int RenderDistance() { return Instance().m_RenderDistance; }
    static void SetRenderDistance(int RenderDistance) { Instance().m_RenderDistance = RenderDistance; }

  private:
    World() {}
    World(World const &);
    void operator=(World const &) {}

    // This will be a temporary cache of the loaded chunks.
    // Functionallity to check for saved data on disk will eventually be
    // implemented.
    std::unordered_map<glm::ivec3, Chunk *> m_ChunkDataMap;
    std::unordered_map<glm::ivec3, Chunk *> m_ChunkRenderMap;

    std::unordered_set<glm::ivec3> m_ChunksToDelete;
    std::unordered_set<glm::ivec3> m_ChunksToGenerate;

    std::unordered_set<glm::ivec3> m_ChunksToRender;
    std::unordered_set<glm::ivec3> m_ChunksToUnrender;

    ShaderProgram m_Shader;
    Texture m_Texture;
    int m_Seed = 0;
    int m_RenderDistance = 0;

    std::mutex m_Lock;

    std::thread m_Thread;
};
