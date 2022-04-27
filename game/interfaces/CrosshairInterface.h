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
    CrosshairInterface()
    {
        InterfaceHandler::AddInterface(this);
        m_ShaderProgram.Create("..\\game\\shaders\\crosshair.vert", "..\\game\\shaders\\crosshair.frag");
        m_Texture.Create("..\\game\\textures\\crosshair.png");

        m_Indices.insert(m_Indices.end(), {0, 1, 2, 2, 3, 0});
        m_Vertices.insert(m_Vertices.end(), {
                                                Vertex{{-0.0225f, -0.04f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                                                Vertex{{0.0225f, -0.04f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                                                Vertex{{0.0225f, 0.04f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                                                Vertex{{-0.0225f, 0.04f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                                            });

        m_Mesh = Mesh(&m_Vertices, &m_Indices, &m_ShaderProgram);
        m_Mesh.Initialize();
    }
    ~CrosshairInterface() {}

    void Draw() override
    {
        m_Texture.Bind();
        m_Mesh.Bind();

        glUniform1i(glGetUniformLocation(m_ShaderProgram.GetID(), "u_Texture"), 0);

        glEnable(GL_BLEND);
        glDrawElements(GL_TRIANGLES, m_Mesh.Count(), GL_UNSIGNED_INT, (void *)0);
        glDisable(GL_BLEND);
    }

  private:
    std::vector <Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    ShaderProgram m_ShaderProgram;
    Texture m_Texture;
    Mesh m_Mesh;
};
