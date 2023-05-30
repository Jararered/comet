#pragma once

#include "Vertex.hpp"

#include <vector>

struct Geometry
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    unsigned int Offset;
};
