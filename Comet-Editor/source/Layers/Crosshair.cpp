#include "Crosshair.h"

#include <filesystem>

Crosshair::Crosshair()
{
    // Get current working directory
    std::filesystem::path path = std::filesystem::current_path();

    // Get the absolute path of the shader files
    std::string vertexShaderPath = path.string() + "/Resources/Shaders/PositionTexture.vert";
    std::string fragmentShaderPath = path.string() + "/Resources/Shaders/PositionTexture.frag";
    m_Shader.Create(vertexShaderPath, fragmentShaderPath);

    // Get the absolute path of the texture file
    std::string texturePath = path.string() + "/Resources/Textures/crosshair.png";
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
