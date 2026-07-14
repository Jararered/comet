#include "Mesh.h"

#include <glm/ext/matrix_transform.hpp>

namespace
{
void PopulateRaylibMesh(::Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    const size_t vertexCount = indices.size();

    mesh = {0};
    mesh.vertexCount = static_cast<int>(vertexCount);
    mesh.triangleCount = static_cast<int>(vertexCount / 3);

    mesh.vertices = static_cast<float*>(RL_MALLOC(vertexCount * 3 * sizeof(float)));
    mesh.texcoords = static_cast<float*>(RL_MALLOC(vertexCount * 2 * sizeof(float)));
    mesh.normals = static_cast<float*>(RL_MALLOC(vertexCount * 3 * sizeof(float)));
    mesh.indices = nullptr;

    for (size_t i = 0; i < vertexCount; i++)
    {
        const Vertex& vertex = vertices[indices[i]];

        mesh.vertices[i * 3 + 0] = vertex.Position.x;
        mesh.vertices[i * 3 + 1] = vertex.Position.y;
        mesh.vertices[i * 3 + 2] = vertex.Position.z;

        mesh.texcoords[i * 2 + 0] = vertex.TextureCoordinate.x;
        mesh.texcoords[i * 2 + 1] = vertex.TextureCoordinate.y;

        mesh.normals[i * 3 + 0] = vertex.Normal.x;
        mesh.normals[i * 3 + 1] = vertex.Normal.y;
        mesh.normals[i * 3 + 2] = vertex.Normal.z;
    }
}
}

GameMesh::GameMesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, GameShader* shader)
    : m_Vertices(vertices ? *vertices : std::vector<Vertex>{}), m_Indices(indices ? *indices : std::vector<unsigned int>{}), p_Shader(shader), m_OnGPU(false), m_ModelMatrix(1.0f)
{
}

void GameMesh::Bind()
{
}

void GameMesh::Unbind()
{
}

void GameMesh::Initialize()
{
    PopulateRaylibMesh(m_RaylibMesh, m_Vertices, m_Indices);
    UploadMesh(&m_RaylibMesh, false);
    m_OnGPU = true;
}

void GameMesh::Update()
{
}

void GameMesh::UpdateGeometry()
{
    if (m_OnGPU)
    {
        UnloadMesh(m_RaylibMesh);
    }

    PopulateRaylibMesh(m_RaylibMesh, m_Vertices, m_Indices);
    UploadMesh(&m_RaylibMesh, false);
    m_OnGPU = true;
}

void GameMesh::Finalize()
{
    if (m_OnGPU)
    {
        UnloadMesh(m_RaylibMesh);
        m_OnGPU = false;
    }
}
