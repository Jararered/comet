#pragma once

#include "Shader.h"
#include "Vertex.h"

#include <raylib.h>

#include <glm/mat4x4.hpp>

#include <vector>

class GameMesh
{
public:
    GameMesh() = default;
    GameMesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, GameShader* shader);

    void Initialize();
    void Update();
    void UpdateGeometry();
    void Finalize();
    void Bind();
    void Unbind();

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    GameShader* p_Shader = nullptr;

    ::Mesh m_RaylibMesh = {0};
    bool m_OnGPU = false;

    glm::mat4 m_ModelMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

public:
    GameShader* GetShader() const { return p_Shader; }
    const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }
    ::Mesh& GetRaylibMesh() { return m_RaylibMesh; }
};
