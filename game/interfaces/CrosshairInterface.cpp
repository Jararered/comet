#include "CrosshairInterface.h"

CrosshairInterface::CrosshairInterface()
{
    InterfaceHandler::AddInterface(this);
    m_ShaderProgram.Create("..\\game\\shaders\\PositionTexture.vert", "..\\game\\shaders\\PositionTexture.frag");
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

CrosshairInterface::~CrosshairInterface() {}

void CrosshairInterface::Draw()
{
    m_Texture.Bind();
    m_Mesh.Bind();

    glUniform1i(glGetUniformLocation(m_ShaderProgram.GetID(), "u_Texture"), 0);

    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, m_Mesh.Indices()->size(), GL_UNSIGNED_INT, (void *)0);
    glDisable(GL_BLEND);
}
