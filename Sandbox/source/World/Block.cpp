#include "World/Block.hpp"

#include "World/BlockLibrary.hpp"

void Block::RenderCubeBlock(const Block& block, const glm::ivec3& coord, const std::array<bool, 6>& faces, Geometry* geometry)
{
    unsigned int& offset = geometry->Offset;
    std::array<unsigned char, 6> blockTextures = Blocks::GetTextures(block.ID);
    int x = coord.x;
    int y = coord.y;
    int z = coord.z;

    // +X Quad
    if (faces[0])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        offset = offset + 4;
    }

    // -X Quad
    if (faces[1])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        offset = offset + 4;
    }

    // +Y Quad
    if (faces[2])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::BottomRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::BottomLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        offset = offset + 4;
    }

    // -Y Quad
    if (faces[3])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        offset = offset + 4;
    }

    // +Z Quad
    if (faces[4])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        offset = offset + 4;
    }

    // -Z Quad
    if (faces[5])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        offset = offset + 4;
    }
}

void Block::RenderFlowerBlock(const Block& block, const glm::ivec3& coord, Geometry* geometry)
{
    unsigned int& offset = geometry->Offset;
    std::array<unsigned char, 6> blockTextures = Blocks::GetTextures(block.ID);
    int x = coord.x;
    int y = coord.y;
    int z = coord.z;

    geometry->Indices.insert(geometry->Indices.end(), {
        0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset,
        4 + offset, 5 + offset, 6 + offset, 6 + offset, 7 + offset, 4 + offset,
        8 + offset, 9 + offset, 10 + offset, 10 + offset, 11 + offset, 8 + offset,
        12 + offset, 13 + offset, 14 + offset, 14 + offset, 15 + offset, 12 + offset });

    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});

    offset += 16;
}

void Block::RenderTorchBlock(const Block& block, const glm::ivec3& coord, Geometry* geometry)
{
    unsigned int& offset = geometry->Offset;
    std::array<unsigned char, 6> blockTextures = Blocks::GetTextures(block.ID);
    int x = coord.x;
    int y = coord.y;
    int z = coord.z;

    // +X Quad
    geometry->Indices.insert(geometry->Indices.end(), {
            0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset,
            4 + offset, 5 + offset, 6 + offset, 6 + offset, 7 + offset, 4 + offset,
            8 + offset, 9 + offset, 10 + offset, 10 + offset, 11 + offset, 8 + offset,
            12 + offset, 13 + offset, 14 + offset, 14 + offset, 15 + offset, 12 + offset,
            16 + offset, 17 + offset, 18 + offset, 18 + offset, 19 + offset, 16 + offset,
            20 + offset, 21 + offset, 22 + offset, 22 + offset, 23 + offset, 20 + offset });
    geometry->Vertices.emplace_back(glm::vec3{x + 0.0625f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.0625f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.0625f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.0625f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.0625f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.0625f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.0625f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.0625f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.125f, z + 0.5f}, TextureMap::TopLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.125f, z - 0.5f}, TextureMap::TopRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.125f, z - 0.5f}, TextureMap::BottomRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.125f, z + 0.5f}, TextureMap::BottomLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z + 0.0625f}, TextureMap::TopRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z + 0.0625f}, TextureMap::TopLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.0625f}, TextureMap::BottomLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.0625f}, TextureMap::BottomRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + 0.5f, z - 0.0625f}, TextureMap::TopRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.0625f}, TextureMap::BottomRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.0625f}, TextureMap::BottomLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + 0.5f, z - 0.0625f}, TextureMap::TopLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
    offset += 24;
}

void Block::RenderSlabBlock(const Block& block, const glm::ivec3& coord, const std::array<bool, 6>& faces, Geometry* geometry)
{
    unsigned int& offset = geometry->Offset;
    std::array<unsigned char, 6> blockTextures = Blocks::GetTextures(block.ID);

    int x = coord.x;
    int y = coord.y;
    int z = coord.z;

    // +X Quad
    if (faces[0])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y, z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y, z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        offset = offset + 4;
    }

    // -X Quad
    if (faces[1])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y, z + 0.5f}, TextureMap::TopRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y, z - 0.5f}, TextureMap::TopLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        offset = offset + 4;
    }

    // +Y Quad
    geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y, z + 0.5f}, TextureMap::TopLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y, z - 0.5f}, TextureMap::TopRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y, z - 0.5f}, TextureMap::BottomRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y, z + 0.5f}, TextureMap::BottomLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
    offset = offset + 4;

    // -Y Quad
    if (faces[3])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        offset = offset + 4;
    }

    // +Z Quad
    if (faces[4])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y, z + 0.5f}, TextureMap::TopRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y, z + 0.5f}, TextureMap::TopLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        offset = offset + 4;
    }

    // -Z Quad
    if (faces[5])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y, z - 0.5f}, TextureMap::TopRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, glm::vec2{0.0f, -0.5f / 16.0f} + TextureMap::BottomLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y, z - 0.5f}, TextureMap::TopLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        offset = offset + 4;
    }
}

void Block::RenderWaterBlock(const Block& block, const glm::ivec3& coord, const std::array<bool, 6>& faces, Geometry* geometry)
{
    unsigned int& offset = geometry->Offset;
    std::array<unsigned char, 6> blockTextures = Blocks::GetTextures(block.ID);

    int x = coord.x;
    int y = coord.y;
    int z = coord.z;

    // +X Quad
    if (faces[0])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + (6.0f / 16.0f), z + 0.5f}, TextureMap::TopRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + (6.0f / 16.0f), z - 0.5f}, TextureMap::TopLeft(blockTextures[0]), glm::vec3{1.0f, 0.0f, 0.0f});
        offset = offset + 4;
    }

    // -X Quad
    if (faces[1])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + (6.0f / 16.0f), z + 0.5f}, TextureMap::TopRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + (6.0f / 16.0f), z - 0.5f}, TextureMap::TopLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[1]), glm::vec3{-1.0f, 0.0f, 0.0f});
        offset = offset + 4;
    }

    // +Y Quad
    if (faces[2])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + (6.0f / 16.0f), z + 0.5f}, TextureMap::TopLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + (6.0f / 16.0f), z - 0.5f}, TextureMap::TopRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + (6.0f / 16.0f), z - 0.5f}, TextureMap::BottomRight(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + (6.0f / 16.0f), z + 0.5f}, TextureMap::BottomLeft(blockTextures[2]), glm::vec3{0.0f, 1.0f, 0.0f});
        offset = offset + 4;
    }

    // -Y Quad
    if (faces[3])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::TopRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::TopLeft(blockTextures[3]), glm::vec3{0.0f, -1.0f, 0.0f});
        offset = offset + 4;
    }

    // +Z Quad
    if (faces[4])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + (6.0f / 16.0f), z + 0.5f}, TextureMap::TopRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + (6.0f / 16.0f), z + 0.5f}, TextureMap::TopLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomLeft(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockTextures[4]), glm::vec3{0.0f, 0.0f, 1.0f});
        offset = offset + 4;
    }

    // -Z Quad
    if (faces[5])
    {
        geometry->Indices.insert(geometry->Indices.end(), { 0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset });
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y + (6.0f / 16.0f), z - 0.5f}, TextureMap::TopRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomRight(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        geometry->Vertices.emplace_back(glm::vec3{x - 0.5f, y + (6.0f / 16.0f), z - 0.5f}, TextureMap::TopLeft(blockTextures[5]), glm::vec3{0.0f, 0.0f, -1.0f});
        offset = offset + 4;
    }
}
