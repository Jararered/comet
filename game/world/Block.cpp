#include "Block.h"

#include "BlockLibrary.h"

void Block::RenderCubeBlock(Block currentBlock, glm::ivec3 Coordinate, std::array<bool, 6> BlockFacesToRender,
                     Geometry *geometry)
{
    unsigned int &offset = geometry->Offset;
    std::array<unsigned char, 6> blockIndices = Blocks::GetIndices(currentBlock.ID);
    int x = Coordinate.x;
    int y = Coordinate.y;
    int z = Coordinate.z;

    // +X Quad
    if (BlockFacesToRender[0])
    {
        geometry->Indices.insert(geometry->Indices.end(),
                                 {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset});
        geometry->Vertices.insert(
            geometry->Vertices.end(),
            {
                Vertex({x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
                Vertex({x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
                Vertex({x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
                Vertex({x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            });
        offset += 4;
    }

    // -X Quad
    if (BlockFacesToRender[1])
    {
        geometry->Indices.insert(geometry->Indices.end(),
                                 {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset});
        geometry->Vertices.insert(
            geometry->Vertices.end(),
            {
                Vertex({x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockIndices[1]), {-1.0f, 0.0f, 0.0f}),
                Vertex({x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockIndices[1]), {-1.0f, 0.0f, 0.0f}),
                Vertex({x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockIndices[1]), {-1.0f, 0.0f, 0.0f}),
                Vertex({x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockIndices[1]), {-1.0f, 0.0f, 0.0f}),
            });
        offset += 4;
    }

    // +Y Quad
    if (BlockFacesToRender[2])
    {
        geometry->Indices.insert(geometry->Indices.end(),
                                 {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset});
        geometry->Vertices.insert(
            geometry->Vertices.end(),
            {
                Vertex({x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopLeft(blockIndices[2]), {0.0f, 1.0f, 0.0f}),
                Vertex({x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopRight(blockIndices[2]), {0.0f, 1.0f, 0.0f}),
                Vertex({x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::BottomRight(blockIndices[2]), {0.0f, 1.0f, 0.0f}),
                Vertex({x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::BottomLeft(blockIndices[2]), {0.0f, 1.0f, 0.0f}),
            });
        offset += 4;
    }

    // -Y Quad
    if (BlockFacesToRender[3])
    {
        geometry->Indices.insert(geometry->Indices.end(),
                                 {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset});
        geometry->Vertices.insert(
            geometry->Vertices.end(),
            {
                Vertex({x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::TopRight(blockIndices[3]), {0.0f, -1.0f, 0.0f}),
                Vertex({x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockIndices[3]), {0.0f, -1.0f, 0.0f}),
                Vertex({x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockIndices[3]), {0.0f, -1.0f, 0.0f}),
                Vertex({x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::TopLeft(blockIndices[3]), {0.0f, -1.0f, 0.0f}),
            });
        offset += 4;
    }

    // +Z Quad
    if (BlockFacesToRender[4])
    {
        geometry->Indices.insert(geometry->Indices.end(),
                                 {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset});
        geometry->Vertices.insert(
            geometry->Vertices.end(),
            {
                Vertex({x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockIndices[4]), {0.0f, 0.0f, 1.0f}),
                Vertex({x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopLeft(blockIndices[4]), {0.0f, 0.0f, 1.0f}),
                Vertex({x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomLeft(blockIndices[4]), {0.0f, 0.0f, 1.0f}),
                Vertex({x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockIndices[4]), {0.0f, 0.0f, 1.0f}),
            });
        offset += 4;
    }

    // -Z Quad
    if (BlockFacesToRender[5])
    {
        geometry->Indices.insert(geometry->Indices.end(),
                                 {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset});
        geometry->Vertices.insert(
            geometry->Vertices.end(),
            {
                Vertex({x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopRight(blockIndices[5]), {0.0f, 0.0f, -1.0f}),
                Vertex({x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomRight(blockIndices[5]), {0.0f, 0.0f, -1.0f}),
                Vertex({x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockIndices[5]), {0.0f, 0.0f, -1.0f}),
                Vertex({x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockIndices[5]), {0.0f, 0.0f, -1.0f}),
            });
        offset += 4;
    }
}

void Block::RenderFlowerBlock(Block currentBlock, glm::ivec3 Coordinate, Geometry *geometry)
{
    unsigned int &offset = geometry->Offset;
    std::array<unsigned char, 6> blockIndices = Blocks::GetIndices(currentBlock.ID);
    int x = Coordinate.x;
    int y = Coordinate.y;
    int z = Coordinate.z;

    geometry->Indices.insert(geometry->Indices.end(),
                             {0 + offset, 1 + offset, 2 + offset, 2 + offset, 3 + offset, 0 + offset, 4 + offset,
                              5 + offset, 6 + offset, 6 + offset, 7 + offset, 4 + offset});
    geometry->Vertices.insert(
        geometry->Vertices.end(),
        {
            Vertex({x + 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            Vertex({x + 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            Vertex({x - 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            Vertex({x - 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            Vertex({x - 0.5f, y + 0.5f, z + 0.5f}, TextureMap::TopRight(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            Vertex({x - 0.5f, y - 0.5f, z + 0.5f}, TextureMap::BottomRight(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            Vertex({x + 0.5f, y - 0.5f, z - 0.5f}, TextureMap::BottomLeft(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
            Vertex({x + 0.5f, y + 0.5f, z - 0.5f}, TextureMap::TopLeft(blockIndices[0]), {1.0f, 0.0f, 0.0f}),
        });

    offset += 8;
}
