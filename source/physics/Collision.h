#pragma once

#include <comet.pch>

struct Collision
{
    // AABBCC method of collision detection

    float PositiveX;
    float NegativeX;
    float PositiveY;
    float NegativeY;
    float PositiveZ;
    float NegativeZ;

    // Returns the centered bounding box values for a given position and dimensions
    Collision CollisionCenteredXYZ(glm::vec3 position, float xspan, float yspan, float zspan)
    {
        PositiveX = position.x + 0.5f * xspan;
        NegativeX = position.x - 0.5f * xspan;
        PositiveY = position.y + 0.5f * yspan;
        NegativeY = position.y - 0.5f * yspan;
        PositiveZ = position.z + 0.5f * zspan;
        NegativeZ = position.z - 0.5f * zspan;
    }

    // Returns true if the collision boxes intersect at all
    bool Intersect(Collision a, Collision b)
    {
        return (a.NegativeX < b.PositiveX && a.PositiveX > b.NegativeX) &&
               (a.NegativeY < b.PositiveY && a.PositiveY > b.NegativeY) &&
               (a.NegativeZ < b.PositiveZ && a.PositiveZ > b.NegativeZ);
    }
};
