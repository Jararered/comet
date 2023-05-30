#pragma once

#include <Renderer/Geometry.hpp>
#include <Renderer/TextureMap.hpp>

struct Block
{
    Block() {}
    Block(unsigned char id, bool isTransparent, bool isSolid, unsigned int shape) : ID(id), IsTransparent(isTransparent), IsSolid(isSolid), Shape(shape) {}

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

    static void RenderCubeBlock(const Block& block, const glm::ivec3& coord, const std::array<bool, 6>& faces, Geometry* geometry);
    static void RenderFlowerBlock(const Block& block, const glm::ivec3& coord, Geometry* geometry);
    static void RenderTorchBlock(const Block& block, const glm::ivec3& coord, Geometry* geometry);
    static void RenderSlabBlock(const Block& block, const glm::ivec3& coord, const std::array<bool, 6>& faces, Geometry* geometry);
    static void RenderWaterBlock(const Block& block, const glm::ivec3& coord, const std::array<bool, 6>& faces, Geometry* geometry);

    template <class Archive> void serialize(Archive& ar) { ar(ID); }
};
