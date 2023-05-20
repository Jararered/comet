#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
    Vertex(glm::vec3 position, glm::vec2 textureCoordinate, glm::vec3 normal) : Position(position), TextureCoordinate(textureCoordinate), Normal(normal) {}
    glm::vec3 Position;
    glm::vec2 TextureCoordinate;
    glm::vec3 Normal;
};
