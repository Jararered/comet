#pragma once

#include "Shader.hpp"
#include "Vertex.hpp"

#include <glm/mat4x4.hpp>

class Mesh
{
public:
    Mesh();
    Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, Shader* shader);
    ~Mesh();

    void Bind();
    void Unbind();

    void Initialize();
    void Update();
    void UpdateGeometry();
    void Finalize();

private:
    std::vector<Vertex>* p_Vertices;
    std::vector<unsigned int>* p_Indices;
    Shader* p_Shader = nullptr;

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_IBO = 0;
    double m_Transparency = 0.0;

    glm::mat4 m_ModelMatrix = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    bool m_PushedToGPU = false;
    double m_TimeCreated = 0.0;
    double m_TimeDelta = 0.0;

public:
    Shader* GetShader() const { return p_Shader; }
    void SetShader(Shader* Shader) { p_Shader = Shader; }

    std::vector<Vertex>* Vertices() const { return p_Vertices; }
    void SetVertices(std::vector<Vertex>* Vertices) { p_Vertices = Vertices; }

    std::vector<unsigned int>* Indices() const { return p_Indices; }
    void SetIndices(std::vector<unsigned int>* Indices) { p_Indices = Indices; }

    glm::mat4 ModelMatrix() const { return m_ModelMatrix; }
    void SetModelMatrix(const glm::mat4& ModelMatrix) { m_ModelMatrix = ModelMatrix; }

    bool IsPushedToGPU() const { return m_PushedToGPU; }
    void SetPushedToGPU(bool PushedToGPU) { m_PushedToGPU = PushedToGPU; }

    double TimeCreated() const { return m_TimeCreated; }
    void SetTimeCreated(double TimeCreated) { m_TimeCreated = TimeCreated; }

    double Dt() const { return m_TimeDelta; }
    void SetTimeDelta(double Dt) { m_TimeDelta = Dt; }

    double Transparency() const { return m_Transparency; }
    void SetTransparency(double Transparency) { m_Transparency = Transparency; }
};
