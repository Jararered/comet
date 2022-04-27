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
    static Collision CollisionCenteredXYZ(glm::vec3 position, float xspan, float yspan, float zspan)
    {
        return Collision{
            position.x + 0.5f * xspan, position.x - 0.5f * xspan, position.y + 0.5f * yspan,
            position.y - 0.5f * yspan, position.z + 0.5f * zspan, position.z - 0.5f * zspan,
        };
    }

    static bool Intersect(Collision a, Collision b)
    {
        return (a.NegativeX < b.PositiveX && a.PositiveX > b.NegativeX) &&
               (a.NegativeY < b.PositiveY && a.PositiveY > b.NegativeY) &&
               (a.NegativeZ < b.PositiveZ && a.PositiveZ > b.NegativeZ);
    }

    static void FixX(glm::vec3 &position, Collision a, Collision b)
    {
        // X
        if (a.NegativeX < b.PositiveX)
        {
            position.x += b.PositiveX - a.NegativeX;
        }
        else if (a.PositiveX > b.NegativeX)
        {
            position.x -= a.PositiveX - b.NegativeX;
        }
    }

    static void FixY(glm::vec3 &position, Collision a, Collision b)
    {
        // Y
        if (a.NegativeY < b.PositiveY)
        {
            position.y += b.PositiveY - a.NegativeY;
        }
        else if (a.PositiveY > b.NegativeY)
        {
            position.y -= a.PositiveY - b.NegativeY;
        }
    }

    static void FixZ(glm::vec3 &position, Collision a, Collision b)
    {
        // Z
        if (a.NegativeZ < b.PositiveZ)
        {
            position.z += b.PositiveZ - a.NegativeZ;
        }
        else if (a.PositiveZ > b.NegativeZ)
        {
            position.z -= a.PositiveZ - b.NegativeZ;
        }
    }
};
