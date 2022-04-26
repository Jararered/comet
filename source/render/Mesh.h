#pragma once

#include <comet.pch>

#include "ShaderProgram.h"
#include "Vertex.h"

class Mesh
{
  public:
    Mesh();
    Mesh(std::vector<Vertex> *vertices, std::vector<unsigned int> *indices, ShaderProgram *shader);
    ~Mesh();

    void Bind();
    void Unbind();

    void Initialize();
    void Update();
    void UpdateGeometry();
    void Finalize();

  private:
    std::vector<Vertex> *m_Vertices;
    std::vector<unsigned int> *m_Indices;

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_VBOSize = 0;
    unsigned int m_IBO = 0;
    unsigned int m_IBOSize = 0;
    ShaderProgram *m_Shader = nullptr;
    unsigned int m_Count = 0;
    double m_Transparency = 0.0;

    glm::mat4 m_ModelMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    bool m_PushedToGPU = false;
    double m_TimeCreated = 0.0;
    double m_TimeDelta = 0.0;

  public:
    ShaderProgram *Shader() const { return m_Shader; }
    void SetShader(ShaderProgram *Shader) { m_Shader = Shader; }

    unsigned int Count() const { return m_Count; }
    void SetCount(unsigned int Count) { m_Count = Count; }

    std::vector<Vertex> *Vertices() const { return m_Vertices; }
    void SetVertices(std::vector<Vertex> *Vertices) { m_Vertices = Vertices; }

    std::vector<unsigned int> *Indices() const { return m_Indices; }
    void SetIndices(std::vector<unsigned int> *Indices) { m_Indices = Indices; }

    glm::mat4 ModelMatrix() const { return m_ModelMatrix; }
    void SetModelMatrix(const glm::mat4 &ModelMatrix) { m_ModelMatrix = ModelMatrix; }

    bool IsPushedToGPU() const { return m_PushedToGPU; }
    void SetPushedToGPU(bool PushedToGPU) { m_PushedToGPU = PushedToGPU; }

    double TimeCreated() const { return m_TimeCreated; }
    void SetTimeCreated(double TimeCreated) { m_TimeCreated = TimeCreated; }

    double TimeDelta() const { return m_TimeDelta; }
    void SetTimeDelta(double TimeDelta) { m_TimeDelta = TimeDelta; }

    double Transparency() const { return m_Transparency; }
    void SetTransparency(double Transparency) { m_Transparency = Transparency; }
};
