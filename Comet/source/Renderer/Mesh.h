#pragma once

#include "Shader.h"
#include "Vertex.h"

#include <glm/mat4x4.hpp>

#include <vector>

class Mesh
{
public:
    Mesh() = default;
    Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, Shader* shader);

    void Initialize();
    void Update();
    void UpdateGeometry();
    void Finalize();
    void Bind();
    void Unbind();

private:
    std::vector<Vertex>* p_Vertices;
    std::vector<unsigned int>* p_Indices;
    Shader* p_Shader = nullptr;

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_IBO = 0;

    glm::mat4 m_ModelMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    bool m_OnGPU = false;

public:
    Shader* GetShader() const { return p_Shader; }
    std::vector<unsigned int>* GetIndices() const { return p_Indices; }
    glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }
};
