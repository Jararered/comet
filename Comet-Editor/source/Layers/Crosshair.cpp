#include "Crosshair.h"

#include "ResourcePaths.h"

Crosshair::Crosshair()
{
    const std::filesystem::path res = EditorResourcesRoot();

    std::string vertexShaderPath = (res / "Shaders" / "PositionTexture.vert").string();
    std::string fragmentShaderPath = (res / "Shaders" / "PositionTexture.frag").string();
    m_Shader.Create(vertexShaderPath, fragmentShaderPath);

    std::string texturePath = (res / "Textures" / "crosshair.png").string();
    m_Texture.Create(texturePath);

    // Create the indices for the crosshair mesh
    m_Indices.insert(m_Indices.end(), {0, 1, 2, 2, 3, 0});

    // Create the vertices for the crosshair mesh
    m_Vertices.insert(m_Vertices.end(), {Vertex{{-0.0225f, -0.04f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, Vertex{{0.0225f, -0.04f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                                         Vertex{{0.0225f, 0.04f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}, Vertex{{-0.0225f, 0.04f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}});

    // Create the crosshair mesh
    m_Mesh = Mesh(&m_Vertices, &m_Indices, &m_Shader);

    // Initialize the crosshair mesh
    m_Mesh.Initialize();
}

void Crosshair::Draw()
{
    // Bind the shader, texture, and mesh
    m_Texture.Bind();
    m_Mesh.Bind();

    // Set the shader uniforms
    glUniform1i(glGetUniformLocation(m_Shader.GetID(), "u_Texture"), 0);

    // Draw the mesh
    glEnable(GL_BLEND);
    glDrawElements(GL_TRIANGLES, m_Mesh.GetIndices()->size(), GL_UNSIGNED_INT, (void*)0);
    glDisable(GL_BLEND);
}
