#pragma once

#include <Comet/Renderer/Renderer.hpp>
#include <Comet/Renderer/Shader.hpp>
#include <Comet/Renderer/Texture.hpp>
#include <Comet/Engine.hpp>

#include "BlockLibrary.hpp"
#include "Chunk.hpp"
#include "ChunkGenerator.hpp"
#include "WorldConfig.hpp"

#include <memory>

enum RenderShape
{
    Square,
    Circle
};

class World
{
public:
    inline static auto& Instance()
    {
        static World instance;
        return instance;
    }

    static void Initialize();
    static void Update();
    static void Finalize();

    static void Generate();
    static void GenerateMesh(glm::ivec3 index);

    static Block GetBlock(glm::ivec3 worldPos);
    static void SetBlock(glm::ivec3 worldPos, Block block);
    static glm::ivec3 GetChunkCoord(glm::ivec3 worldPos);
    static glm::ivec3 GetChunkIndex(glm::ivec3 worldPos);

    static void SetShader(const Shader& shader) { Instance().m_Shader = shader; }
    static void SetSeed(int seed) { ChunkGenerator::SetSeed(seed); }
    static int Seed() { return Instance().m_Seed; }
    static void ProcessRequestedChunks(glm::ivec3 centerChunkIndex);

    // Shader Functions
    const Shader& GetShader() { return m_Shader; }

    static int RenderDistance() { return Instance().m_RenderDistance; }
    static void SetRenderDistance(int RenderDistance) { Instance().m_RenderDistance = RenderDistance; }

    static int RenderShape() { return Instance().m_RenderShape; }
    static void SetRenderShape(int RenderShape) { Instance().m_RenderShape = RenderShape; }

private:
    World() {}
    World(World const&);
    void operator=(World const&) {}

    // This will be a temporary cache of the loaded chunks.
    // Functionallity to check for saved data on disk will eventually be
    // implemented.
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkDataMap;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_ChunkRenderMap;

    std::unordered_set<glm::ivec3> m_ChunksToDelete;
    std::unordered_set<glm::ivec3> m_ChunksToGenerate;

    std::unordered_set<glm::ivec3> m_ChunksToRender;
    std::unordered_set<glm::ivec3> m_ChunksToUnrender;

    Shader m_Shader;
    Texture m_Texture;
    int m_Seed = 0;
    int m_RenderDistance = 0;
    int m_RenderShape = RenderShape::Square;

    bool m_Running;

    std::mutex m_Lock;

    std::thread m_Thread;
};
