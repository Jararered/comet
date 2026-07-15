#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
    Vertex() = default;
    Vertex(glm::vec3 position, glm::vec2 textureCoordinate, glm::vec3 normal)
        : Vertex(position, textureCoordinate, normal, 255)
    {
    }

    Vertex(glm::vec3 position, glm::vec2 textureCoordinate, glm::vec3 normal, std::uint8_t ambientOcclusion)
        : PackedPosition(PackPosition(position)), PackedTextureU(PackTextureCoordinate(textureCoordinate.x)), PackedTextureV(PackTextureCoordinate(textureCoordinate.y)), PackedNormal(PackNormal(normal)), AmbientOcclusion(ambientOcclusion)
    {
    }

    glm::vec3 Position() const
    {
        return {
            DecodePositionComponent(PackedPosition & PositionXZMask),
            DecodePositionComponent((PackedPosition >> PositionYShift) & PositionYMask),
            DecodePositionComponent((PackedPosition >> PositionZShift) & PositionXZMask),
        };
    }

    glm::vec2 TextureCoordinate() const
    {
        return {
            DecodeTextureCoordinate(PackedTextureU),
            DecodeTextureCoordinate(PackedTextureV),
        };
    }

    glm::vec3 Normal() const
    {
        switch (PackedNormal)
        {
            case 0:
                return {1.0f, 0.0f, 0.0f};
            case 1:
                return {-1.0f, 0.0f, 0.0f};
            case 2:
                return {0.0f, 1.0f, 0.0f};
            case 3:
                return {0.0f, -1.0f, 0.0f};
            case 4:
                return {0.0f, 0.0f, 1.0f};
            default:
                return {0.0f, 0.0f, -1.0f};
        }
    }

    std::uint32_t PackedPosition = 0;
    std::int16_t PackedTextureU = 0;
    std::int16_t PackedTextureV = 0;
    std::uint8_t PackedNormal = 4;
    std::uint8_t AmbientOcclusion = 255;

private:
    static constexpr int PositionScale = 16;
    static constexpr int PositionBias = 8;
    static constexpr std::uint32_t PositionXZMask = 0x1ff;
    static constexpr std::uint32_t PositionYMask = 0x3fff;
    static constexpr int PositionZShift = 9;
    static constexpr int PositionYShift = 18;
    static constexpr float TextureScale = 1024.0f;

    static std::uint32_t PackPosition(glm::vec3 position)
    {
        const std::uint32_t x = PackPositionComponent(position.x, PositionXZMask);
        const std::uint32_t y = PackPositionComponent(position.y, PositionYMask);
        const std::uint32_t z = PackPositionComponent(position.z, PositionXZMask);

        return x | (z << PositionZShift) | (y << PositionYShift);
    }

    static std::uint32_t PackPositionComponent(float value, std::uint32_t mask)
    {
        const int encoded = static_cast<int>(std::lround(value * static_cast<float>(PositionScale))) + PositionBias;
        return static_cast<std::uint32_t>(std::clamp(encoded, 0, static_cast<int>(mask)));
    }

    static float DecodePositionComponent(std::uint32_t value)
    {
        return static_cast<float>(static_cast<int>(value) - PositionBias) / static_cast<float>(PositionScale);
    }

    static std::int16_t PackTextureCoordinate(float value)
    {
        const int encoded = static_cast<int>(std::lround(value * TextureScale));
        return static_cast<std::int16_t>(std::clamp(encoded, static_cast<int>(std::numeric_limits<std::int16_t>::min()), static_cast<int>(std::numeric_limits<std::int16_t>::max())));
    }

    static float DecodeTextureCoordinate(std::int16_t value)
    {
        return static_cast<float>(value) / TextureScale;
    }

    static std::uint8_t PackNormal(glm::vec3 normal)
    {
        const glm::vec3 absolute = {std::abs(normal.x), std::abs(normal.y), std::abs(normal.z)};
        if (absolute.x >= absolute.y && absolute.x >= absolute.z)
        {
            return normal.x >= 0.0f ? 0 : 1;
        }
        if (absolute.y >= absolute.z)
        {
            return normal.y >= 0.0f ? 2 : 3;
        }
        return normal.z >= 0.0f ? 4 : 5;
    }

};

static_assert(sizeof(Vertex) <= 12);
