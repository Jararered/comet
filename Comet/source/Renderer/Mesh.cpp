#include "Mesh.h"

#include <glm/ext/matrix_transform.hpp>

namespace
{
void ReleaseRaylibCpuMeshData(::Mesh& mesh)
{
    if (mesh.vertices)
    {
        RL_FREE(mesh.vertices);
        mesh.vertices = nullptr;
    }
    if (mesh.texcoords)
    {
        RL_FREE(mesh.texcoords);
        mesh.texcoords = nullptr;
    }
    if (mesh.normals)
    {
        RL_FREE(mesh.normals);
        mesh.normals = nullptr;
    }
}

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
        const glm::vec3 position = vertex.Position();
        const glm::vec2 textureCoordinate = vertex.TextureCoordinate();
        const glm::vec3 normal = vertex.Normal();

        mesh.vertices[i * 3 + 0] = position.x;
        mesh.vertices[i * 3 + 1] = position.y;
        mesh.vertices[i * 3 + 2] = position.z;

        mesh.texcoords[i * 2 + 0] = textureCoordinate.x;
        mesh.texcoords[i * 2 + 1] = textureCoordinate.y;

        mesh.normals[i * 3 + 0] = normal.x;
        mesh.normals[i * 3 + 1] = normal.y;
        mesh.normals[i * 3 + 2] = normal.z;
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
    ReleaseRaylibCpuMeshData(m_RaylibMesh);
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
    ReleaseRaylibCpuMeshData(m_RaylibMesh);
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
