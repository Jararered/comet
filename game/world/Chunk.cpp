#include "Chunk.h"

#include "World.h"
#include "world/WorldConfig.h"

Chunk::Chunk(glm::ivec3 id) : m_Chunk(id) {}

Chunk::~Chunk()
{
    if (m_Modified)
    {
        std::string filename = ".\\worlddata\\" + std::to_string(m_Chunk.x) + " " + std::to_string(m_Chunk.z) + ".bin";
        std::ofstream os(filename, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(m_BlockData);
    }
}

void Chunk::Allocate()
{
    m_BlockData.fill(Blocks::Air());
    m_HeightData.fill(0);

    m_Geometry.Vertices.reserve(10000);
    m_Geometry.Indices.reserve(10000);
}

void Chunk::Generate()
{
    std::string filename = ".\\worlddata\\" + std::to_string(m_Chunk.x) + " " + std::to_string(m_Chunk.z) + ".bin";
    if (std::filesystem::exists(filename))
    {
        std::cout << "Chunk file found, loading chunk...\n";
        std::ifstream is(filename, std::ios::binary);
        cereal::BinaryInputArchive archive(is);
        archive(m_BlockData);

        for (auto &block : m_BlockData)
        {
            block = Blocks::GetBlockFromID(block.ID);
        }

        SetGenerated(false);
    }
    else
    {
        // World Generation
        GenerateSurface();
        GenerateBedrock();
        GenerateTrees();
        GenerateFlowers();
        GenerateSand();
        GenerateWater();

        SetGenerated(true);
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
                SetBlock({x - 2, y + 3, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x - 2, y + 3, z}, Blocks::Oak_Leaves());
                SetBlock({x - 2, y + 3, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 3, z - 2}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 3, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 3, z}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 3, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 3, z + 2}, Blocks::Oak_Leaves());
                SetBlock({x, y + 3, z - 2}, Blocks::Oak_Leaves());
                SetBlock({x, y + 3, z - 1}, Blocks::Oak_Leaves());

                SetBlock({x, y + 3, z}, Blocks::Oak_Log());

                SetBlock({x, y + 3, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x, y + 3, z + 2}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 3, z - 2}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 3, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 3, z}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 3, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 3, z + 2}, Blocks::Oak_Leaves());
                SetBlock({x + 2, y + 3, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x + 2, y + 3, z}, Blocks::Oak_Leaves());
                SetBlock({x + 2, y + 3, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x - 2, y + 4, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x - 2, y + 4, z}, Blocks::Oak_Leaves());
                SetBlock({x - 2, y + 4, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 4, z - 2}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 4, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 4, z}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 4, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x - 1, y + 4, z + 2}, Blocks::Oak_Leaves());
                SetBlock({x, y + 4, z - 2}, Blocks::Oak_Leaves());
                SetBlock({x, y + 4, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x, y + 4, z}, Blocks::Oak_Leaves());
                SetBlock({x, y + 4, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x, y + 4, z + 2}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 4, z - 2}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 4, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 4, z}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 4, z + 1}, Blocks::Oak_Leaves());
                SetBlock({x + 1, y + 4, z + 2}, Blocks::Oak_Leaves());
                SetBlock({x + 2, y + 4, z - 1}, Blocks::Oak_Leaves());
                SetBlock({x + 2, y + 4, z}, Blocks::Oak_Leaves());
                SetBlock({x + 2, y + 4, z + 1}, Blocks::Oak_Leaves());
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
            float noise3 =
                ChunkGenerator::GetFastNoise((m_Chunk.x * CHUNK_WIDTH) + x + 2, (m_Chunk.z * CHUNK_WIDTH) + z);

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
    m_Geometry.Vertices.clear();
    m_Geometry.Indices.clear();
    m_Geometry.Offset = 0;

    Block currentBlock, pxBlock, nxBlock, pyBlock, nyBlock, pzBlock, nzBlock;
    bool px = false, nx = false, py = false, ny = false, pz = false, nz = false;
    int worldx = 0, worldz = 0;

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < CHUNK_WIDTH; z++)
            {
                currentBlock = GetBlock({x, y, z});

                // If the block is air, add no geometry
                if (currentBlock.ID == ID::Air)
                {
                    continue;
                }

                worldx = x + m_Chunk.x * CHUNK_WIDTH;
                worldz = z + m_Chunk.z * CHUNK_WIDTH;

                // Getting block IDs of surrounding blocks
                if (x == 0)
                {
                    pxBlock = GetBlock({x + 1, y, z});
                    nxBlock = World::GetBlock({worldx - 1, y, worldz});
                }
                else
                {
                    if (x == CHUNK_WIDTH - 1)
                    {
                        pxBlock = World::GetBlock({worldx + 1, y, worldz});
                        nxBlock = GetBlock({x - 1, y, z});
                    }
                    else
                    {
                        pxBlock = GetBlock({x + 1, y, z});
                        nxBlock = GetBlock({x - 1, y, z});
                    }
                }

                pyBlock = GetBlock({x, y + 1, z});
                nyBlock = GetBlock({x, y - 1, z});

                if (z == 0)
                {
                    pzBlock = GetBlock({x, y, z + 1});
                    nzBlock = World::GetBlock({worldx, y, worldz - 1});
                }
                else
                {
                    if (z == CHUNK_WIDTH - 1)
                    {
                        pzBlock = World::GetBlock({worldx, y, worldz + 1});
                        nzBlock = GetBlock({x, y, z - 1});
                    }
                    else
                    {
                        pzBlock = GetBlock({x, y, z + 1});
                        nzBlock = GetBlock({x, y, z - 1});
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
                }

                if (currentBlock.Shape == Block::Shapes::Cube)
                {
                    Block::RenderCubeBlock(currentBlock, {x, y, z}, {px, nx, py, ny, pz, nz}, &m_Geometry);
                    continue;
                }
                if (currentBlock.Shape == Block::Shapes::Flower)
                {
                    Block::RenderFlowerBlock(currentBlock, {x, y, z}, &m_Geometry);
                    continue;
                }
                if (currentBlock.Shape == Block::Shapes::Torch)
                {
                    Block::RenderTorchBlock(currentBlock, {x, y, z}, &m_Geometry);
                    continue;
                }
                if (currentBlock.Shape == Block::Shapes::Slab)
                {
                    Block::RenderSlabBlock(currentBlock, {x, y, z}, {px, nx, py, ny, pz, nz}, &m_Geometry);
                    continue;
                }
            }
        }
    }
}
