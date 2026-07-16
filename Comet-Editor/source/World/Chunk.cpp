#include "Chunk.h"

#include "World.h"
#include "WorldConfig.h"
#include <Profiler/Profiler.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace
{
    struct GreedyFace
    {
        bool Visible = false;
        unsigned char BlockID = ID::Air;
        std::array<std::uint8_t, 4> AmbientOcclusion = {255, 255, 255, 255};
    };


    bool IsGreedyCubeBlock(const Block& block)
    {
        return block.ID != ID::Air && block.ID != ID::Water && block.Shape == Block::Shapes::Cube && !block.IsTransparent;
    }

    bool CanMergeFaces(const GreedyFace& a, const GreedyFace& b)
    {
        return a.Visible == b.Visible && a.BlockID == b.BlockID && a.AmbientOcclusion == b.AmbientOcclusion;
    }

    std::uint8_t CalculateAmbientOcclusion(bool side1, bool side2, bool corner)
    {
        const int occlusion = side1 && side2 ? 3 : static_cast<int>(side1) + static_cast<int>(side2) + static_cast<int>(corner);
        constexpr std::array<std::uint8_t, 4> lightLevels = {105, 150, 200, 255};
        return lightLevels[3 - occlusion];
    }

    void AddGreedyQuad(Geometry* geometry, unsigned char blockID, int face, float minX, float minY, float minZ, float maxX, float maxY, float maxZ, const std::array<std::uint8_t, 4>& ambientOcclusion)
    {
        unsigned int& offset = geometry->Offset;
        std::array<unsigned char, 6> blockTextures = Blocks::GetTextures(blockID);
        const unsigned char texture = blockTextures[face];
        const float tileWidth = (face == 0 || face == 1 || face == 2 || face == 3) ? maxZ - minZ : maxX - minX;
        const float tileHeight = (face == 2 || face == 3) ? maxX - minX : maxY - minY;
        const int atlasWidth = 16;
        const float encodedTileX = static_cast<float>(texture % atlasWidth);
        const float encodedTileY = static_cast<float>(texture / atlasWidth);
        constexpr float greedyUvScale = 1024.0f;
        auto encodeGreedyUv = [encodedTileX, encodedTileY](float u, float v) {
            return glm::vec2{-(encodedTileX + 1.0f) - (u / greedyUvScale), -(encodedTileY + 1.0f) - (v / greedyUvScale)};
        };

        const glm::vec2 topLeft = encodeGreedyUv(0.0f, 0.0f);
        const glm::vec2 topRight = encodeGreedyUv(tileWidth, 0.0f);
        const glm::vec2 bottomLeft = encodeGreedyUv(0.0f, tileHeight);
        const glm::vec2 bottomRight = encodeGreedyUv(tileWidth, tileHeight);

        if (ambientOcclusion[0] + ambientOcclusion[2] > ambientOcclusion[1] + ambientOcclusion[3])
        {
            geometry->Indices.insert(geometry->Indices.end(), {0 + offset, 1 + offset, 3 + offset, 1 + offset, 2 + offset, 3 + offset});
        }
        else
        {
            geometry->Indices.insert(geometry->Indices.end(), {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset});
        }

        switch (face)
        {
            case 0:
                geometry->Vertices.emplace_back(glm::vec3{maxX, maxY, maxZ}, topRight, glm::vec3{1.0f, 0.0f, 0.0f}, ambientOcclusion[0]);
                geometry->Vertices.emplace_back(glm::vec3{maxX, minY, maxZ}, bottomRight, glm::vec3{1.0f, 0.0f, 0.0f}, ambientOcclusion[1]);
                geometry->Vertices.emplace_back(glm::vec3{maxX, minY, minZ}, bottomLeft, glm::vec3{1.0f, 0.0f, 0.0f}, ambientOcclusion[2]);
                geometry->Vertices.emplace_back(glm::vec3{maxX, maxY, minZ}, topLeft, glm::vec3{1.0f, 0.0f, 0.0f}, ambientOcclusion[3]);
                break;
            case 1:
                geometry->Vertices.emplace_back(glm::vec3{minX, maxY, maxZ}, topRight, glm::vec3{-1.0f, 0.0f, 0.0f}, ambientOcclusion[0]);
                geometry->Vertices.emplace_back(glm::vec3{minX, maxY, minZ}, topLeft, glm::vec3{-1.0f, 0.0f, 0.0f}, ambientOcclusion[1]);
                geometry->Vertices.emplace_back(glm::vec3{minX, minY, minZ}, bottomLeft, glm::vec3{-1.0f, 0.0f, 0.0f}, ambientOcclusion[2]);
                geometry->Vertices.emplace_back(glm::vec3{minX, minY, maxZ}, bottomRight, glm::vec3{-1.0f, 0.0f, 0.0f}, ambientOcclusion[3]);
                break;
            case 2:
                geometry->Vertices.emplace_back(glm::vec3{maxX, maxY, maxZ}, topLeft, glm::vec3{0.0f, 1.0f, 0.0f}, ambientOcclusion[0]);
                geometry->Vertices.emplace_back(glm::vec3{maxX, maxY, minZ}, topRight, glm::vec3{0.0f, 1.0f, 0.0f}, ambientOcclusion[1]);
                geometry->Vertices.emplace_back(glm::vec3{minX, maxY, minZ}, bottomRight, glm::vec3{0.0f, 1.0f, 0.0f}, ambientOcclusion[2]);
                geometry->Vertices.emplace_back(glm::vec3{minX, maxY, maxZ}, bottomLeft, glm::vec3{0.0f, 1.0f, 0.0f}, ambientOcclusion[3]);
                break;
            case 3:
                geometry->Vertices.emplace_back(glm::vec3{maxX, minY, maxZ}, topRight, glm::vec3{0.0f, -1.0f, 0.0f}, ambientOcclusion[0]);
                geometry->Vertices.emplace_back(glm::vec3{minX, minY, maxZ}, bottomRight, glm::vec3{0.0f, -1.0f, 0.0f}, ambientOcclusion[1]);
                geometry->Vertices.emplace_back(glm::vec3{minX, minY, minZ}, bottomLeft, glm::vec3{0.0f, -1.0f, 0.0f}, ambientOcclusion[2]);
                geometry->Vertices.emplace_back(glm::vec3{maxX, minY, minZ}, topLeft, glm::vec3{0.0f, -1.0f, 0.0f}, ambientOcclusion[3]);
                break;
            case 4:
                geometry->Vertices.emplace_back(glm::vec3{maxX, maxY, maxZ}, topRight, glm::vec3{0.0f, 0.0f, 1.0f}, ambientOcclusion[0]);
                geometry->Vertices.emplace_back(glm::vec3{minX, maxY, maxZ}, topLeft, glm::vec3{0.0f, 0.0f, 1.0f}, ambientOcclusion[1]);
                geometry->Vertices.emplace_back(glm::vec3{minX, minY, maxZ}, bottomLeft, glm::vec3{0.0f, 0.0f, 1.0f}, ambientOcclusion[2]);
                geometry->Vertices.emplace_back(glm::vec3{maxX, minY, maxZ}, bottomRight, glm::vec3{0.0f, 0.0f, 1.0f}, ambientOcclusion[3]);
                break;
            case 5:
                geometry->Vertices.emplace_back(glm::vec3{maxX, maxY, minZ}, topRight, glm::vec3{0.0f, 0.0f, -1.0f}, ambientOcclusion[0]);
                geometry->Vertices.emplace_back(glm::vec3{maxX, minY, minZ}, bottomRight, glm::vec3{0.0f, 0.0f, -1.0f}, ambientOcclusion[1]);
                geometry->Vertices.emplace_back(glm::vec3{minX, minY, minZ}, bottomLeft, glm::vec3{0.0f, 0.0f, -1.0f}, ambientOcclusion[2]);
                geometry->Vertices.emplace_back(glm::vec3{minX, maxY, minZ}, topLeft, glm::vec3{0.0f, 0.0f, -1.0f}, ambientOcclusion[3]);
                break;
        }

        offset += 4;
    }
}

Chunk::Chunk(World* world, glm::ivec3 id) : m_World(world), m_Chunk(id)
{
}

Chunk::~Chunk()
{
    if (m_ChunkProperties.Modified)
    {
        SaveToDisk();
    }
}

void Chunk::SaveToDisk()
{
    std::string filename = ".\\worlddata\\" + std::to_string(m_Chunk.x) + " " + std::to_string(m_Chunk.z) + ".bin";
    std::ofstream os(filename, std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    archive(m_BlockData);
}

void Chunk::Allocate()
{
    m_BlockData.fill(Blocks::Air());
    m_HeightData.fill(0);

    m_Geometry.Vertices.reserve(10000);
    m_Geometry.Indices.reserve(10000);
    m_WaterGeometry.Vertices.reserve(1000);
    m_WaterGeometry.Indices.reserve(1000);
}

void Chunk::LoadFromDisk(const std::string& filename)
{
    std::ifstream is(filename, std::ios::binary);
    cereal::BinaryInputArchive archive(is);
    archive(m_BlockData);

    for (auto& block : m_BlockData)
    {
        block = Blocks::GetBlockFromID(block.ID);
    }
}

void Chunk::Generate()
{
    COMET_PROFILE_SCOPE("Chunk::Generate", "world_generation");
    std::string filename = ".\\worlddata\\" + std::to_string(m_Chunk.x) + " " + std::to_string(m_Chunk.z) + ".bin";
    if (std::filesystem::exists(filename))
    {
        std::cout << "Chunk::Generate(): Chunk file found, loading chunk...\n";
        LoadFromDisk(filename);
        m_ChunkProperties.Generated = false;
    }
    else
    {
        // Generate world
        GenerateSurface();
        GenerateBedrock();
        GenerateTrees();
        GenerateFlowers();
        GenerateSand();
        GenerateWater();

        m_ChunkProperties.Generated = true;
    }
}

void Chunk::GenerateSurface()
{
    float height = 0.0f;
    float noise1, noise2, noise3, noise4, noise5;
    int y = 0;

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            // Calculating a surface height with the noise
            noise1 = ChunkGenerator::GetPerlin1((m_Chunk.x * CHUNK_WIDTH) + x, (m_Chunk.z * CHUNK_WIDTH) + z);
            noise2 = ChunkGenerator::GetPerlin2((m_Chunk.x * CHUNK_WIDTH) + x, (m_Chunk.z * CHUNK_WIDTH) + z);
            noise3 = ChunkGenerator::GetPerlin4((m_Chunk.x * CHUNK_WIDTH) + x, (m_Chunk.z * CHUNK_WIDTH) + z);
            noise4 = ChunkGenerator::GetPerlin8((m_Chunk.x * CHUNK_WIDTH) + x, (m_Chunk.z * CHUNK_WIDTH) + z);

            height = 40.0f + (20.0f * noise1) + (10.0f * noise2) + (5.0f * noise3) + (2.5f * noise4);
            y = static_cast<int>(height);

            SetHeight(x, z, height);

            SetBlock({x, y, z}, Blocks::Grass());
            SetBlock({x, y - 1, z}, Blocks::Dirt());
            SetBlock({x, y - 2, z}, Blocks::Dirt());
            SetBlock({x, y - 3, z}, Blocks::Dirt());

            // fill chunk under dirt with stone
            for (int i = 0; i < y - 3; i++)
            {
                SetBlock({x, i, z}, Blocks::Stone());
            }
        }
    }
}

void Chunk::GenerateTrees()
{
    float noise1;
    float noise2;
    int y;
    int water_height = 30;
    int mountain_height = 70;

    for (int x = 2; x < CHUNK_WIDTH - 2; x++)
    {
        for (int z = 2; z < CHUNK_WIDTH - 2; z++)
        {
            y = GetHeight(x, z);

            // Check if generating a tree above water level
            if (y < water_height + 3 || water_height > mountain_height)
                continue;

            // Check to not generate a floating tree
            if (GetBlock({x, y - 1, z}).ID == ID::Air)
                continue;

            noise1 = ChunkGenerator::GetFastNoise((m_Chunk.x * CHUNK_WIDTH) + x, (m_Chunk.z * CHUNK_WIDTH) + z);
            noise2 = ChunkGenerator::GetMediumNoise((m_Chunk.x * CHUNK_WIDTH) + x, (m_Chunk.z * CHUNK_WIDTH) + z);

            if (noise1 > 0.9f && noise2 > 0.1f)
            {
                SetBlock({x, y, z}, Blocks::Dirt());
                SetBlock({x, y + 1, z}, Blocks::Oak_Log());
                SetBlock({x, y + 2, z}, Blocks::Oak_Log());

                if (noise1 > 0.95f)
                {
                    SetBlock({x, y + 3, z}, Blocks::Oak_Log());
                    y += 1;
                }

                for (int xoffset = -2; xoffset < 3; xoffset++)
                {
                    for (int yoffset = -2; yoffset < 3; yoffset++)
                    {
                        SetBlock({x + xoffset, y + 3, z + yoffset}, Blocks::Oak_Leaves());
                    }
                }

                SetBlock({x, y + 3, z}, Blocks::Oak_Log());

                for (int xoffset = -2; xoffset < 3; xoffset++)
                {
                    for (int yoffset = -2; yoffset < 3; yoffset++)
                    {
                        SetBlock({x + xoffset, y + 4, z + yoffset}, Blocks::Oak_Leaves());
                    }
                }

                SetBlock({x - 1, y + 5, z}, Blocks::Oak_Leaves());
                SetBlock({x, y + 5, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x, y + 5, z}, Blocks::Oak_Leaves());
                SetBlock({x, y + 5, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 5, z}, Blocks::Oak_Leaves());

                SetBlock({x - 1, y + 6, z}, Blocks::Oak_Leaves());
                SetBlock({x, y + 6, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x, y + 6, z}, Blocks::Oak_Leaves());
                SetBlock({x, y + 6, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 6, z}, Blocks::Oak_Leaves());
            }
        }
    }
}

void Chunk::GenerateFlowers()
{
    float noise1;
    float noise2;
    int y;
    int water_height = 30;
    int mountain_height = 70;

    for (int x = 2; x < CHUNK_WIDTH - 2; x++)
    {
        for (int z = 2; z < CHUNK_WIDTH - 2; z++)
        {
            y = GetHeight(x, z);

            // Check if generating a tree above water level
            if (y < water_height + 3 || water_height > mountain_height)
            {
                continue;
            }

            // Check to not generate a floating tree
            if (GetBlock({x, y - 1, z}).ID == ID::Air)
            {
                continue;
            }

            noise1 = ChunkGenerator::GetFastNoise((m_Chunk.x * CHUNK_WIDTH) + x + 1, (m_Chunk.z * CHUNK_WIDTH) + z);
            noise2 = ChunkGenerator::GetMediumNoise((m_Chunk.x * CHUNK_WIDTH) + x + 1, (m_Chunk.z * CHUNK_WIDTH) + z);
            float noise3 = ChunkGenerator::GetFastNoise((m_Chunk.x * CHUNK_WIDTH) + x + 2, (m_Chunk.z * CHUNK_WIDTH) + z);

            if (noise1 > 0.75f && noise2 > 0.1f)
            {
                if (GetBlock({x, y + 1, z}).ID == ID::Air)
                {
                    if (noise3 < 0.25f)
                    {
                        SetBlock({x, y + 1, z}, Blocks::Red_Flower());
                        continue;
                    }
                    if (noise3 < 0.5f)
                    {
                        SetBlock({x, y + 1, z}, Blocks::Yellow_Flower());
                        continue;
                    }
                    if (noise3 < 0.75f)
                    {
                        SetBlock({x, y + 1, z}, Blocks::Red_Mushroom());
                        continue;
                    }
                    SetBlock({x, y + 1, z}, Blocks::Brown_Mushroom());
                    continue;
                }
            }
        }
    }
}

void Chunk::GenerateBedrock()
{
    float noise;
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            noise = ChunkGenerator::GetFastNoise(x, z);

            SetBlock({x, 0, z}, Blocks::Bedrock());

            if (noise > 0.20f)
                SetBlock({x, 1, z}, Blocks::Bedrock());
            else
                continue;
            if (noise > 0.40f)
                SetBlock({x, 2, z}, Blocks::Bedrock());
            else
                continue;
            if (noise > 0.60f)
                SetBlock({x, 3, z}, Blocks::Bedrock());
            else
                continue;
            if (noise > 0.80f)
                SetBlock({x, 4, z}, Blocks::Bedrock());
        }
    }
}

void Chunk::GenerateWater()
{
    int y;
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            y = GetHeight(x, z);
            if (y < WATER_HEIGHT + 1)
            {
                for (unsigned int i = y + 1; i < WATER_HEIGHT + 1; i++)
                {
                    SetBlock({x, i, z}, Blocks::Water());
                }
            }
        }
    }
}

void Chunk::GenerateSand()
{
    int y;
    float noise;
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            y = GetHeight(x, z);
            noise = ChunkGenerator::GetBiomeNoise(x, z);
            if (y < WATER_HEIGHT + 4)
            {
                SetBlock({x, y, z}, Blocks::Sand());
                SetBlock({x, y - 1, z}, Blocks::Sand());
                SetBlock({x, y - 2, z}, Blocks::Sand());
            }

            if (y == WATER_HEIGHT + 4 && noise > 0.5f)
            {
                SetBlock({x, y, z}, Blocks::Sand());
                SetBlock({x, y - 1, z}, Blocks::Sand());
                SetBlock({x, y - 2, z}, Blocks::Sand());
            }
        }
    }
}

void Chunk::GenerateMesh()
{
    COMET_PROFILE_SCOPE("Chunk::GenerateMesh", "world_meshing");
    m_Geometry.Vertices.clear();
    m_Geometry.Indices.clear();
    m_Geometry.Offset = 0;
    m_WaterGeometry.Vertices.clear();
    m_WaterGeometry.Indices.clear();
    m_WaterGeometry.Offset = 0;

    int minBlockY = CHUNK_HEIGHT;
    int maxBlockY = -1;
    int minFallbackY = CHUNK_HEIGHT;
    int maxFallbackY = -1;
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_WIDTH; z++)
            {
                const Block& block = m_BlockData[x * CHUNK_HEIGHT * CHUNK_WIDTH + y * CHUNK_WIDTH + z];
                if (block.ID != ID::Air)
                {
                    minBlockY = std::min(minBlockY, y);
                    maxBlockY = std::max(maxBlockY, y);

                    if (!IsGreedyCubeBlock(block))
                    {
                        minFallbackY = std::min(minFallbackY, y);
                        maxFallbackY = std::max(maxFallbackY, y);
                    }
                }
            }
        }
    }

    if (maxBlockY < minBlockY)
    {
        return;
    }

    auto getLocalBlock = [this](int x, int y, int z) -> Block {
        if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH)
        {
            return Blocks::Air();
        }

        return m_BlockData[x * CHUNK_HEIGHT * CHUNK_WIDTH + y * CHUNK_WIDTH + z];
    };

    auto getNeighborBlock = [this, &getLocalBlock](int x, int y, int z) -> Block {
        if (y < 0 || y >= CHUNK_HEIGHT)
        {
            return Blocks::Air();
        }

        if (x < 0 || x >= CHUNK_WIDTH || z < 0 || z >= CHUNK_WIDTH)
        {
            return m_World->GetBlockForMeshing({x + m_Chunk.x * CHUNK_WIDTH, y, z + m_Chunk.z * CHUNK_WIDTH});
        }

        return getLocalBlock(x, y, z);
    };

    std::array<GreedyFace, CHUNK_WIDTH * CHUNK_HEIGHT> greedyMask;
    std::array<bool, CHUNK_WIDTH * CHUNK_HEIGHT> greedyConsumed;
    auto emitGreedyMask = [&greedyMask, &greedyConsumed](int width, int height, auto&& getFace, auto&& emitFace) {
        for (int v = 0; v < height; v++)
        {
            for (int u = 0; u < width; u++)
            {
                greedyMask[v * width + u] = getFace(u, v);
                greedyConsumed[v * width + u] = false;
            }
        }

        for (int v = 0; v < height; v++)
        {
            for (int u = 0; u < width; u++)
            {
                const int index = v * width + u;
                if (greedyConsumed[index] || !greedyMask[index].Visible)
                {
                    continue;
                }

                int quadWidth = 1;
                while (u + quadWidth < width)
                {
                    const int nextIndex = v * width + u + quadWidth;
                    if (greedyConsumed[nextIndex] || !CanMergeFaces(greedyMask[index], greedyMask[nextIndex]))
                    {
                        break;
                    }
                    quadWidth++;
                }

                int quadHeight = 1;
                bool canGrow = true;
                while (v + quadHeight < height && canGrow)
                {
                    for (int offset = 0; offset < quadWidth; offset++)
                    {
                        const int nextIndex = (v + quadHeight) * width + u + offset;
                        if (greedyConsumed[nextIndex] || !CanMergeFaces(greedyMask[index], greedyMask[nextIndex]))
                        {
                            canGrow = false;
                            break;
                        }
                    }

                    if (canGrow)
                    {
                        quadHeight++;
                    }
                }

                for (int y = 0; y < quadHeight; y++)
                {
                    for (int x = 0; x < quadWidth; x++)
                    {
                        greedyConsumed[(v + y) * width + u + x] = true;
                    }
                }

                emitFace(u, v, quadWidth, quadHeight, greedyMask[index]);
            }
        }
    };

    auto occludesAmbient = [](const Block& block) {
        return IsGreedyCubeBlock(block);
    };

    auto calculateCornerAO = [&getNeighborBlock, &occludesAmbient](int x, int y, int z, glm::ivec3 normal, glm::ivec3 side1, glm::ivec3 side2) {
        const bool sideBlock1 = occludesAmbient(getNeighborBlock(x + normal.x + side1.x, y + normal.y + side1.y, z + normal.z + side1.z));
        const bool sideBlock2 = occludesAmbient(getNeighborBlock(x + normal.x + side2.x, y + normal.y + side2.y, z + normal.z + side2.z));
        const bool cornerBlock = occludesAmbient(getNeighborBlock(x + normal.x + side1.x + side2.x, y + normal.y + side1.y + side2.y, z + normal.z + side1.z + side2.z));
        return CalculateAmbientOcclusion(sideBlock1, sideBlock2, cornerBlock);
    };

    auto calculateFaceAO = [&calculateCornerAO](int face, int x, int y, int z) -> std::array<std::uint8_t, 4> {
        switch (face)
        {
            case 0:
                return {
                    calculateCornerAO(x, y, z, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}),
                    calculateCornerAO(x, y, z, {1, 0, 0}, {0, -1, 0}, {0, 0, 1}),
                    calculateCornerAO(x, y, z, {1, 0, 0}, {0, -1, 0}, {0, 0, -1}),
                    calculateCornerAO(x, y, z, {1, 0, 0}, {0, 1, 0}, {0, 0, -1}),
                };
            case 1:
                return {
                    calculateCornerAO(x, y, z, {-1, 0, 0}, {0, 1, 0}, {0, 0, 1}),
                    calculateCornerAO(x, y, z, {-1, 0, 0}, {0, 1, 0}, {0, 0, -1}),
                    calculateCornerAO(x, y, z, {-1, 0, 0}, {0, -1, 0}, {0, 0, -1}),
                    calculateCornerAO(x, y, z, {-1, 0, 0}, {0, -1, 0}, {0, 0, 1}),
                };
            case 2:
                return {
                    calculateCornerAO(x, y, z, {0, 1, 0}, {1, 0, 0}, {0, 0, 1}),
                    calculateCornerAO(x, y, z, {0, 1, 0}, {1, 0, 0}, {0, 0, -1}),
                    calculateCornerAO(x, y, z, {0, 1, 0}, {-1, 0, 0}, {0, 0, -1}),
                    calculateCornerAO(x, y, z, {0, 1, 0}, {-1, 0, 0}, {0, 0, 1}),
                };
            case 3:
                return {
                    calculateCornerAO(x, y, z, {0, -1, 0}, {1, 0, 0}, {0, 0, 1}),
                    calculateCornerAO(x, y, z, {0, -1, 0}, {-1, 0, 0}, {0, 0, 1}),
                    calculateCornerAO(x, y, z, {0, -1, 0}, {-1, 0, 0}, {0, 0, -1}),
                    calculateCornerAO(x, y, z, {0, -1, 0}, {1, 0, 0}, {0, 0, -1}),
                };
            case 4:
                return {
                    calculateCornerAO(x, y, z, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}),
                    calculateCornerAO(x, y, z, {0, 0, 1}, {-1, 0, 0}, {0, 1, 0}),
                    calculateCornerAO(x, y, z, {0, 0, 1}, {-1, 0, 0}, {0, -1, 0}),
                    calculateCornerAO(x, y, z, {0, 0, 1}, {1, 0, 0}, {0, -1, 0}),
                };
            default:
                return {
                    calculateCornerAO(x, y, z, {0, 0, -1}, {1, 0, 0}, {0, 1, 0}),
                    calculateCornerAO(x, y, z, {0, 0, -1}, {1, 0, 0}, {0, -1, 0}),
                    calculateCornerAO(x, y, z, {0, 0, -1}, {-1, 0, 0}, {0, -1, 0}),
                    calculateCornerAO(x, y, z, {0, 0, -1}, {-1, 0, 0}, {0, 1, 0}),
                };
        }
    };

    auto buildFace = [&getLocalBlock, &getNeighborBlock, &calculateFaceAO](int x, int y, int z, int faceIndex, int dx, int dy, int dz) {
        GreedyFace greedyFace;
        const Block block = getLocalBlock(x, y, z);
        const Block neighbor = getNeighborBlock(x + dx, y + dy, z + dz);

        if (IsGreedyCubeBlock(block) && neighbor.IsTransparent)
        {
            greedyFace.Visible = true;
            greedyFace.BlockID = block.ID;
            greedyFace.AmbientOcclusion = calculateFaceAO(faceIndex, x, y, z);
        }

        return greedyFace;
    };

    const int meshMinY = minBlockY;
    const int meshMaxY = maxBlockY;
    const int meshHeight = meshMaxY - meshMinY + 1;

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        emitGreedyMask(
            CHUNK_WIDTH, meshHeight, [&](int z, int y) { return buildFace(x, meshMinY + y, z, 0, 1, 0, 0); },
            [&](int z, int y, int width, int height, const GreedyFace& face) {
                y += meshMinY;
                AddGreedyQuad(&m_Geometry, face.BlockID, 0, x - 0.5f, y - 0.5f, z - 0.5f, x + 0.5f, y + height - 0.5f, z + width - 0.5f, face.AmbientOcclusion);
            });

        emitGreedyMask(
            CHUNK_WIDTH, meshHeight, [&](int z, int y) { return buildFace(x, meshMinY + y, z, 1, -1, 0, 0); },
            [&](int z, int y, int width, int height, const GreedyFace& face) {
                y += meshMinY;
                AddGreedyQuad(&m_Geometry, face.BlockID, 1, x - 0.5f, y - 0.5f, z - 0.5f, x + 0.5f, y + height - 0.5f, z + width - 0.5f, face.AmbientOcclusion);
            });
    }

    for (int y = meshMinY; y <= meshMaxY; y++)
    {
        emitGreedyMask(
            CHUNK_WIDTH, CHUNK_WIDTH, [&](int x, int z) { return buildFace(x, y, z, 2, 0, 1, 0); },
            [&](int x, int z, int width, int height, const GreedyFace& face) {
                AddGreedyQuad(&m_Geometry, face.BlockID, 2, x - 0.5f, y - 0.5f, z - 0.5f, x + width - 0.5f, y + 0.5f, z + height - 0.5f, face.AmbientOcclusion);
            });

        emitGreedyMask(
            CHUNK_WIDTH, CHUNK_WIDTH, [&](int x, int z) { return buildFace(x, y, z, 3, 0, -1, 0); },
            [&](int x, int z, int width, int height, const GreedyFace& face) {
                AddGreedyQuad(&m_Geometry, face.BlockID, 3, x - 0.5f, y - 0.5f, z - 0.5f, x + width - 0.5f, y + 0.5f, z + height - 0.5f, face.AmbientOcclusion);
            });
    }

    for (int z = 0; z < CHUNK_WIDTH; z++)
    {
        emitGreedyMask(
            CHUNK_WIDTH, meshHeight, [&](int x, int y) { return buildFace(x, meshMinY + y, z, 4, 0, 0, 1); },
            [&](int x, int y, int width, int height, const GreedyFace& face) {
                y += meshMinY;
                AddGreedyQuad(&m_Geometry, face.BlockID, 4, x - 0.5f, y - 0.5f, z - 0.5f, x + width - 0.5f, y + height - 0.5f, z + 0.5f, face.AmbientOcclusion);
            });

        emitGreedyMask(
            CHUNK_WIDTH, meshHeight, [&](int x, int y) { return buildFace(x, meshMinY + y, z, 5, 0, 0, -1); },
            [&](int x, int y, int width, int height, const GreedyFace& face) {
                y += meshMinY;
                AddGreedyQuad(&m_Geometry, face.BlockID, 5, x - 0.5f, y - 0.5f, z - 0.5f, x + width - 0.5f, y + height - 0.5f, z + 0.5f, face.AmbientOcclusion);
            });
    }

    Block currentBlock, pxBlock, nxBlock, pyBlock, nyBlock, pzBlock, nzBlock;
    bool px = false, nx = false, py = false, ny = false, pz = false, nz = false;
    int worldx = 0, worldz = 0;

    if (maxFallbackY < minFallbackY)
    {
        return;
    }

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int y = minFallbackY; y <= maxFallbackY; y++)
        {
            for (int z = 0; z < CHUNK_WIDTH; z++)
            {
                currentBlock = getLocalBlock(x, y, z);

                // If the block is air, add no geometry
                if (currentBlock.ID == ID::Air)
                {
                    continue;
                }

                if (IsGreedyCubeBlock(currentBlock))
                {
                    continue;
                }

                worldx = x + m_Chunk.x * CHUNK_WIDTH;
                worldz = z + m_Chunk.z * CHUNK_WIDTH;

                // Getting block IDs of surrounding blocks
                if (x == 0)
                {
                    pxBlock = getLocalBlock(x + 1, y, z);
                    nxBlock = m_World->GetBlockForMeshing({worldx - 1, y, worldz});
                }
                else
                {
                    if (x == CHUNK_WIDTH - 1)
                    {
                        pxBlock = m_World->GetBlockForMeshing({worldx + 1, y, worldz});
                        nxBlock = getLocalBlock(x - 1, y, z);
                    }
                    else
                    {
                        pxBlock = getLocalBlock(x + 1, y, z);
                        nxBlock = getLocalBlock(x - 1, y, z);
                    }
                }

                pyBlock = getLocalBlock(x, y + 1, z);
                nyBlock = getLocalBlock(x, y - 1, z);

                if (z == 0)
                {
                    pzBlock = getLocalBlock(x, y, z + 1);
                    nzBlock = m_World->GetBlockForMeshing({worldx, y, worldz - 1});
                }
                else
                {
                    if (z == CHUNK_WIDTH - 1)
                    {
                        pzBlock = m_World->GetBlockForMeshing({worldx, y, worldz + 1});
                        nzBlock = getLocalBlock(x, y, z - 1);
                    }
                    else
                    {
                        pzBlock = getLocalBlock(x, y, z + 1);
                        nzBlock = getLocalBlock(x, y, z - 1);
                    }
                }

                if (currentBlock.IsTransparent)
                {
                    px = true;
                    nx = true;
                    py = true;
                    ny = true;
                    pz = true;
                    nz = true;
                }

                // Determining if side should be rendered
                px = pxBlock.IsTransparent;
                nx = nxBlock.IsTransparent;
                py = pyBlock.IsTransparent;
                ny = nyBlock.IsTransparent;
                pz = pzBlock.IsTransparent;
                nz = nzBlock.IsTransparent;

                // Water rendering
                if (currentBlock.ID == ID::Water)
                {
                    px = (pxBlock.ID == ID::Water || !pxBlock.IsTransparent) ? false : true;
                    nx = (nxBlock.ID == ID::Water || !nxBlock.IsTransparent) ? false : true;
                    py = (pyBlock.ID == ID::Water || !pyBlock.IsTransparent) ? false : true;
                    ny = (nyBlock.ID == ID::Water || !nyBlock.IsTransparent) ? false : true;
                    pz = (pzBlock.ID == ID::Water || !pzBlock.IsTransparent) ? false : true;
                    nz = (nzBlock.ID == ID::Water || !nzBlock.IsTransparent) ? false : true;

                    if (py)
                    {
                        Block::RenderWaterBlock(currentBlock, {x, y, z}, {px, nx, py, ny, pz, nz}, &m_WaterGeometry);
                    }
                    else
                    {
                        Block::RenderCubeBlock(currentBlock, {x, y, z}, {px, nx, py, ny, pz, nz}, &m_WaterGeometry);
                    }
                    continue;
                }

                if (currentBlock.Shape == Block::Shapes::Cube)
                {
                    Block::RenderCubeBlock(currentBlock, {x, y, z}, {px, nx, py, ny, pz, nz}, &m_Geometry);
                    continue;
                }
                if (currentBlock.Shape == Block::Shapes::Cross)
                {
                    Block::RenderFlowerBlock(currentBlock, {x, y, z}, &m_Geometry);
                    continue;
                }
                if (currentBlock.Shape == Block::Shapes::Torch)
                {
                    Block::RenderTorchBlock(currentBlock, {x, y, z}, &m_Geometry);
                    continue;
                }
                if (currentBlock.Shape == Block::Shapes::Half)
                {
                    Block::RenderSlabBlock(currentBlock, {x, y, z}, {px, nx, py, ny, pz, nz}, &m_Geometry);
                    continue;
                }
            }
        }
    }
}
