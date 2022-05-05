#pragma once

#include <comet.pch>

#include "handlers/Interface.h"
#include "handlers/InterfaceHandler.h"
#include "handlers/WindowHandler.h"
#include "render/Mesh.h"
#include "render/ShaderProgram.h"
#include "render/Texture.h"
#include "render/Vertex.h"

class CrosshairInterface : public Interface
{
public:
    CrosshairInterface();
    ~CrosshairInterface();

    void Draw();

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    ShaderProgram m_ShaderProgram;
    Texture m_Texture;
    Mesh m_Mesh;
};
