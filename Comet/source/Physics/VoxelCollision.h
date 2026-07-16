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
            position.x + halfW, position.x - halfW, position.y + headClearance, position.y - height, position.z + halfW, position.z - halfW,
        };
    }

    // Block at integer world cell matches Block::RenderCubeBlock + PositionTextureNormal.vert: cube centered on
    // (cell.x, cell.y, cell.z) with half-extent 0.5 (i.e. [x-0.5,x+0.5] x ...), not min-corner at the integer.
    inline Collision BlockBoundingBox(glm::ivec3 cell)
    {
        const glm::vec3 c{static_cast<float>(cell.x), static_cast<float>(cell.y), static_cast<float>(cell.z)};
        return Collision::BoundingBoxCentered(c, 1.0f, 1.0f, 1.0f);
    }

    // Moves a Verlet body through the voxel field in short steps, then projects it out of every penetration.
    // Updating lastPosition on a projected axis clears the Verlet velocity into that surface.
    template <typename IsSolidFn>
    void ResolveVoxelCollisions(glm::vec3& position, glm::vec3& lastPosition, float width, float height, float headClearance, IsSolidFn&& isSolid, int blockMargin = 1, int maxProjectionIterations = 8,
                                float maxSubstepDistance = 0.2f, int maxSubsteps = 32)
    {
        // A single final-position projection can choose the wrong face after crossing a corner or a thin
        // wall. Substeps retain the cheap Verlet integration while giving the solver a local previous AABB.
        const glm::vec3 targetPosition = position;
        const glm::vec3 movement = targetPosition - lastPosition;
        const int substeps = std::clamp(static_cast<int>(std::ceil(glm::length(movement) / maxSubstepDistance)), 1, maxSubsteps);
        const glm::vec3 substepMovement = movement / static_cast<float>(substeps);
        bool blockedX = false;
        bool blockedY = false;
        bool blockedZ = false;

        position = lastPosition;

        for (int step = 0; step < substeps; ++step)
        {
            const glm::vec3 previousPosition = position;
            position += substepMovement;

            for (int iteration = 0; iteration < maxProjectionIterations; ++iteration)
            {
                const Collision player = PlayerBoundingBox(position, width, height, headClearance);
                const Collision previousPlayer = PlayerBoundingBox(previousPosition, width, height, headClearance);

                const int minX = static_cast<int>(std::floor(player.NegativeX)) - blockMargin;
                const int maxX = static_cast<int>(std::floor(player.PositiveX)) + blockMargin;
                const int minY = static_cast<int>(std::floor(player.NegativeY)) - blockMargin;
                const int maxY = static_cast<int>(std::floor(player.PositiveY)) + blockMargin;
                const int minZ = static_cast<int>(std::floor(player.NegativeZ)) - blockMargin;
                const int maxZ = static_cast<int>(std::floor(player.PositiveZ)) + blockMargin;

                int bestAxis = -1;
                float bestCorrection = 0.0f;
                float smallestCorrection = 0.0f;

                auto consider = [&](int axis, float correction)
                {
                    const float magnitude = std::abs(correction);
                    if (bestAxis < 0 || magnitude < smallestCorrection)
                    {
                        bestAxis = axis;
                        bestCorrection = correction;
                        smallestCorrection = magnitude;
                    }
                };

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
                            if (!Collision::IsIntersect(player, block))
                                continue;

                            const float overlapX = std::min(player.PositiveX, block.PositiveX) - std::max(player.NegativeX, block.NegativeX);
                            const float overlapY = std::min(player.PositiveY, block.PositiveY) - std::max(player.NegativeY, block.NegativeY);
                            const float overlapZ = std::min(player.PositiveZ, block.PositiveZ) - std::max(player.NegativeZ, block.NegativeZ);

                            const bool enteredFromNegativeX = previousPlayer.PositiveX <= block.NegativeX;
                            const bool enteredFromPositiveX = previousPlayer.NegativeX >= block.PositiveX;
                            const bool enteredFromNegativeY = previousPlayer.PositiveY <= block.NegativeY;
                            const bool enteredFromPositiveY = previousPlayer.NegativeY >= block.PositiveY;
                            const bool enteredFromNegativeZ = previousPlayer.PositiveZ <= block.NegativeZ;
                            const bool enteredFromPositiveZ = previousPlayer.NegativeZ >= block.PositiveZ;

                            if (enteredFromNegativeX)
                                consider(0, -overlapX);
                            else if (enteredFromPositiveX)
                                consider(0, overlapX);
                            if (enteredFromNegativeY)
                                consider(1, -overlapY);
                            else if (enteredFromPositiveY)
                                consider(1, overlapY);
                            if (enteredFromNegativeZ)
                                consider(2, -overlapZ);
                            else if (enteredFromPositiveZ)
                                consider(2, overlapZ);

                            if (enteredFromNegativeX || enteredFromPositiveX || enteredFromNegativeY || enteredFromPositiveY || enteredFromNegativeZ || enteredFromPositiveZ)
                                continue;

                            // Recovery from an already-overlapping spawn/edit chooses the smallest translation.
                            const glm::vec3 playerCenter{
                                0.5f * (player.PositiveX + player.NegativeX),
                                0.5f * (player.PositiveY + player.NegativeY),
                                0.5f * (player.PositiveZ + player.NegativeZ),
                            };
                            const glm::vec3 blockCenter{
                                0.5f * (block.PositiveX + block.NegativeX),
                                0.5f * (block.PositiveY + block.NegativeY),
                                0.5f * (block.PositiveZ + block.NegativeZ),
                            };
                            consider(0, playerCenter.x < blockCenter.x ? -overlapX : overlapX);
                            consider(1, playerCenter.y < blockCenter.y ? -overlapY : overlapY);
                            consider(2, playerCenter.z < blockCenter.z ? -overlapZ : overlapZ);
                        }
                    }
                }

                if (bestAxis < 0)
                    break;

                position[bestAxis] += bestCorrection;
                if (bestAxis == 0)
                    blockedX = true;
                else if (bestAxis == 1)
                    blockedY = true;
                else
                    blockedZ = true;
            }
        }

        if (blockedX)
            lastPosition.x = position.x;
        if (blockedY)
            lastPosition.y = position.y;
        if (blockedZ)
            lastPosition.z = position.z;
    }

    template <typename IsSolidFn>
    bool IsGrounded(glm::vec3 position, float width, float height, float headClearance, IsSolidFn&& isSolid, float maxAirGapAboveGround = 0.2f, float maxPenetrationBelowTop = 0.35f)
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
