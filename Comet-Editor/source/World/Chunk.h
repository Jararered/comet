#pragma once

#include <Renderer/Renderer.h>
#include <Renderer/TextureMap.h>
#include <Renderer/Vertex.h>
#include <Timer.h>

#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>

#include "Block.h"
#include "BlockLibrary.h"
#include "ChunkGenerator.h"
#include "WorldConfig.h"

#include <string>

class World;

struct ChunkProperties
{
    bool Modified = false;
    bool Generated = false;
};

struct Chunk
{
    Chunk() = default;
    Chunk(World* world, glm::ivec3 id);
    ~Chunk();

    // For use by world manager
    void Allocate();
    void Generate();
    void SaveToDisk();
    void LoadFromDisk(const std::string& filename);
    ChunkProperties& GetChunkProperties() { return m_ChunkProperties; }

    void GenerateSurface();
    void GenerateTrees();
    void GenerateFlowers();
    void GenerateBedrock();
    void GenerateWater();
    void GenerateSand();

    void GenerateMesh();

    Block GetBlock(glm::ivec3 chunkPos)
    {
        if (chunkPos.x < 0 || chunkPos.y < 0 || chunkPos.z < 0)
        {
            return Blocks::Air();
        }
        if (chunkPos.x == CHUNK_WIDTH || chunkPos.y == CHUNK_HEIGHT || chunkPos.z == CHUNK_WIDTH)
        {
            return Blocks::Air();
        }

        return m_BlockData.at(chunkPos.x * CHUNK_HEIGHT * CHUNK_WIDTH + chunkPos.y * CHUNK_WIDTH + chunkPos.z);
    }

    void SetBlock(glm::ivec3 chunkPos, Block block) { m_BlockData.at(chunkPos.x * CHUNK_HEIGHT * CHUNK_WIDTH + chunkPos.y * CHUNK_WIDTH + chunkPos.z) = block; }

    void SetHeight(int x, int z, int y) { m_HeightData.at(CHUNK_WIDTH * x + z) = y; }
    float GetHeight(int x, int z) { return m_HeightData.at(CHUNK_WIDTH * x + z); }

    Geometry* GetGeometry() { return &m_Geometry; }

private:
    ChunkProperties m_ChunkProperties;

    std::array<Block, CHUNK_SIZE> m_BlockData;

    // only used during generation, not needed when saving chunk
    std::array<int, CHUNK_WIDTH * CHUNK_WIDTH> m_HeightData;

    Geometry m_Geometry;

    World* m_World;

    glm::ivec3 m_Chunk = {0, 0, 0};

public:
    template <class Archive> void save(Archive& ar) const { ar(m_BlockData); }
    template <class Archive> void load(Archive& ar) { ar(m_BlockData); }
};
