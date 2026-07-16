#pragma once

#include "Shader.h"
#include "Vertex.h"

#include <raylib.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vector>

class GameMesh
{
public:
    GameMesh() = default;
    GameMesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, GameShader* shader);
    GameMesh(std::vector<float> vertices, std::vector<float> texcoords, std::vector<float> normals);
    ~GameMesh();

    // GPU resources cannot be shared by two mesh values. Copies deliberately
    // keep only the CPU geometry so queued meshes always receive a fresh upload.
    GameMesh(const GameMesh& other);
    GameMesh& operator=(const GameMesh& other);
    GameMesh(GameMesh&& other) noexcept;
    GameMesh& operator=(GameMesh&& other) noexcept;

    void Initialize();
    void Update();
    void UpdateGeometry();
    void Finalize();
    void Bind();
    void Unbind();

private:
    void UpdateBounds();

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<float> m_ExpandedVertices;
    std::vector<float> m_ExpandedTexcoords;
    std::vector<float> m_ExpandedNormals;
    GameShader* p_Shader = nullptr;

    ::Mesh m_RaylibMesh = {0};
    bool m_OnGPU = false;
    bool m_HasBounds = false;
    glm::vec3 m_BoundsMin = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_BoundsMax = {0.0f, 0.0f, 0.0f};

    glm::mat4 m_ModelMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

public:
    GameShader* GetShader() const { return p_Shader; }
    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<unsigned int>& GetIndices() const { return m_Indices; }
    const std::vector<float>& GetExpandedVertices() const { return m_ExpandedVertices; }
    bool HasExpandedGeometry() const { return !m_ExpandedVertices.empty(); }
    bool HasBounds() const { return m_HasBounds; }
    glm::vec3 BoundsMin() const { return m_BoundsMin; }
    glm::vec3 BoundsMax() const { return m_BoundsMax; }
    glm::mat4 GetModelMatrix() const { return m_ModelMatrix; }
    ::Mesh& GetRaylibMesh() { return m_RaylibMesh; }
};
