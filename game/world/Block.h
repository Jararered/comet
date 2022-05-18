#pragma once

#include <comet.pch>

#include "render/Geometry.h"
#include "render/TextureMap.h"

struct Block
{
    Block() {}
    Block(unsigned char id, bool isTransparent, bool isSolid, unsigned int shape)
        : ID(id), IsTransparent(isTransparent), IsSolid(isSolid), Shape(shape)
    {
    }

    enum Shapes
    {
        Cube,
        Cross,
        Torch,
        Half
    };

    unsigned char ID = 0;
    bool IsTransparent = false;
    bool IsSolid = false;
    unsigned int Shape = Shapes::Cube;

    static void RenderCubeBlock(Block block, glm::ivec3 coord, std::array<bool, 6> faces, Geometry *geometry);
    static void RenderFlowerBlock(Block block, glm::ivec3 coord, Geometry *geometry);
    static void RenderTorchBlock(Block block, glm::ivec3 coord, Geometry *geometry);
    static void RenderSlabBlock(Block block, glm::ivec3 coord, std::array<bool, 6> faces, Geometry *geometry);
    static void RenderWaterBlock(Block block, glm::ivec3 coord, std::array<bool, 6> faces, Geometry *geometry);

    template <class Archive> void serialize(Archive &ar) { ar(ID); }
};
