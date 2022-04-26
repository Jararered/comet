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
        Flower,
        Torch
    };

    unsigned char ID = 0;
    bool IsTransparent = false;
    bool IsSolid = false;
    unsigned int Shape = Shapes::Cube;

    static void RenderCubeBlock(Block currentBlock, glm::ivec3 Coordinate, std::array<bool, 6> BlockFacesToRender,
                                Geometry *geometry);
    static void RenderFlowerBlock(Block currentBlock, glm::ivec3 Coordinate, Geometry *geometry);

    template <class Archive> void serialize(Archive &ar) { ar(ID); }
};
