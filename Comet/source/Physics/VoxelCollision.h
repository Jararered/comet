#pragma once

#include "Collision.h"

#include <glm/vec3.hpp>

#include <algorithm>
#include <cmath>

namespace Comet::Physics
{

// Player anchor matches Player::UpdateBoundingBox: feet at position.y - height, head clearance above position.y.
inline Collision PlayerBoundingBox(glm::vec3 position, float width, float height, float headClearance = 0.25f)
{
    const float halfW = 0.5f * width;
    return Collision{
        position.x + halfW,
        position.x - halfW,
        position.y + headClearance,
        position.y - height,
        position.z + halfW,
        position.z - halfW,
    };
}

// Block at integer world cell matches Block::RenderCubeBlock + PositionTextureNormal.vert: cube centered on
// (cell.x, cell.y, cell.z) with half-extent 0.5 (i.e. [x-0.5,x+0.5] x ...), not min-corner at the integer.
inline Collision BlockBoundingBox(glm::ivec3 cell)
{
    const glm::vec3 c{static_cast<float>(cell.x), static_cast<float>(cell.y), static_cast<float>(cell.z)};
    return Collision::BoundingBoxCentered(c, 1.0f, 1.0f, 1.0f);
}

// Resolves penetration with solid blocks in a margin around the player AABB. Updates lastPosition on each
// separated axis so Verlet velocity along that axis is cleared (no bounce from integration).
template <typename IsSolidFn>
void ResolveVoxelCollisions(
    glm::vec3& position,
    glm::vec3& lastPosition,
    float width,
    float height,
    float headClearance,
    IsSolidFn&& isSolid,
    int blockMargin = 1,
    int maxIterations = 8
)
{
    for (int iter = 0; iter < maxIterations; ++iter)
    {
        const Collision player = PlayerBoundingBox(position, width, height, headClearance);

        const int minX = static_cast<int>(std::floor(player.NegativeX)) - blockMargin;
        const int maxX = static_cast<int>(std::floor(player.PositiveX)) + blockMargin;
        const int minY = static_cast<int>(std::floor(player.NegativeY)) - blockMargin;
        const int maxY = static_cast<int>(std::floor(player.PositiveY)) + blockMargin;
        const int minZ = static_cast<int>(std::floor(player.NegativeZ)) - blockMargin;
        const int maxZ = static_cast<int>(std::floor(player.PositiveZ)) + blockMargin;

        bool resolvedOne = false;

        for (int bz = minZ; bz <= maxZ && !resolvedOne; ++bz)
        {
            for (int by = minY; by <= maxY && !resolvedOne; ++by)
            {
                for (int bx = minX; bx <= maxX && !resolvedOne; ++bx)
                {
                    const glm::ivec3 cell{bx, by, bz};
                    if (!isSolid(cell))
                        continue;

                    const Collision block = BlockBoundingBox(cell);
                    if (!Collision::IsIntersect(player, block))
                        continue;

                    const float overlapX = std::min(player.PositiveX, block.PositiveX) - std::max(player.NegativeX, block.NegativeX);
                    const float overlapY = std::min(player.PositiveY, block.PositiveY) - std::max(player.NegativeY, block.NegativeY);
                    const float overlapZ = std::min(player.PositiveZ, block.PositiveZ) - std::max(player.NegativeZ, block.NegativeZ);

                    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
                        continue;

                    const glm::vec3 aCenter{
                        0.5f * (player.PositiveX + player.NegativeX),
                        0.5f * (player.PositiveY + player.NegativeY),
                        0.5f * (player.PositiveZ + player.NegativeZ),
                    };
                    const glm::vec3 bCenter{
                        0.5f * (block.PositiveX + block.NegativeX),
                        0.5f * (block.PositiveY + block.NegativeY),
                        0.5f * (block.PositiveZ + block.NegativeZ),
                    };

                    // Prefer vertical separation when depths tie so walking on top of blocks is stable.
                    int axis = 1;
                    float depth = overlapY;
                    if (overlapX < depth)
                    {
                        depth = overlapX;
                        axis = 0;
                    }
                    if (overlapZ < depth)
                    {
                        depth = overlapZ;
                        axis = 2;
                    }

                    if (axis == 0)
                    {
                        if (aCenter.x < bCenter.x)
                            position.x -= depth;
                        else
                            position.x += depth;
                        lastPosition.x = position.x;
                    }
                    else if (axis == 1)
                    {
                        if (aCenter.y < bCenter.y)
                            position.y -= depth;
                        else
                            position.y += depth;
                        lastPosition.y = position.y;
                    }
                    else
                    {
                        if (aCenter.z < bCenter.z)
                            position.z -= depth;
                        else
                            position.z += depth;
                        lastPosition.z = position.z;
                    }

                    resolvedOne = true;
                }
            }
        }

        if (!resolvedOne)
            break;
    }
}

template <typename IsSolidFn>
bool IsGrounded(
    glm::vec3 position,
    float width,
    float height,
    float headClearance,
    IsSolidFn&& isSolid,
    float maxAirGapAboveGround = 0.2f,
    float maxPenetrationBelowTop = 0.35f
)
{
    const Collision player = PlayerBoundingBox(position, width, height, headClearance);
    const float footY = player.NegativeY;

    const int minX = static_cast<int>(std::floor(player.NegativeX));
    const int maxX = static_cast<int>(std::floor(player.PositiveX));
    const int minZ = static_cast<int>(std::floor(player.NegativeZ));
    const int maxZ = static_cast<int>(std::floor(player.PositiveZ));
    const int minY = static_cast<int>(std::floor(footY - maxPenetrationBelowTop)) - 1;
    const int maxY = static_cast<int>(std::floor(footY + maxAirGapAboveGround)) + 1;

    for (int bz = minZ; bz <= maxZ; ++bz)
    {
        for (int by = minY; by <= maxY; ++by)
        {
            for (int bx = minX; bx <= maxX; ++bx)
            {
                const glm::ivec3 cell{bx, by, bz};
                if (!isSolid(cell))
                    continue;

                const Collision block = BlockBoundingBox(cell);
                if (!(player.NegativeX < block.PositiveX && player.PositiveX > block.NegativeX && player.NegativeZ < block.PositiveZ && player.PositiveZ > block.NegativeZ))
                    continue;

                const float blockTop = block.PositiveY;
                // After collision resolve, feet often sit a hair above the surface (no AABB overlap with the
                // block), so use vertical tolerance instead of intersecting a thin feet slab with the voxel.
                if (footY <= blockTop + maxAirGapAboveGround && footY >= blockTop - maxPenetrationBelowTop)
                    return true;
            }
        }
    }

    return false;
}

} // namespace Comet::Physics
