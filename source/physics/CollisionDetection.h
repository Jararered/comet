#pragma once

#include <comet.pch>

struct Collision
{
    // AABBCC method of collision detection

    float NegativeX;
    float PositiveX;
    float NegativeY;
    float PositiveY;
    float NegativeZ;
    float PositiveZ;

    // Returns the centered bounding box values for a given position and dimensions
    static Collision CollisionCenteredXYZ(glm::vec3 position, float xspan, float yspan, float zspan)
    {
        return Collision{
            position.x - 0.5f * xspan, position.x + 0.5f * xspan, position.y - 0.5f * yspan,
            position.y + 0.5f * yspan, position.z - 0.5f * zspan, position.z + 0.5f * zspan,
        };
    }

    bool Intersect(Collision a, Collision b)
    {
        return (a.NegativeX <= b.PositiveX && a.PositiveX >= b.NegativeX) &&
               (a.NegativeY <= b.PositiveY && a.PositiveY >= b.NegativeY) &&
               (a.NegativeZ <= b.PositiveZ && a.PositiveZ >= b.NegativeZ);
    }
};
