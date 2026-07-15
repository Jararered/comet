#include "Mesh.h"

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <utility>

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
    if (mesh.colors)
    {
        RL_FREE(mesh.colors);
        mesh.colors = nullptr;
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
    mesh.colors = static_cast<unsigned char*>(RL_MALLOC(vertexCount * 4 * sizeof(unsigned char)));
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
        mesh.colors[i * 4 + 0] = vertex.AmbientOcclusion;
        mesh.colors[i * 4 + 1] = vertex.AmbientOcclusion;
        mesh.colors[i * 4 + 2] = vertex.AmbientOcclusion;
        mesh.colors[i * 4 + 3] = 255;
    }
}

void PopulateRaylibMesh(::Mesh& mesh, const std::vector<float>& vertices, const std::vector<float>& texcoords, const std::vector<float>& normals)
{
    const size_t vertexCount = vertices.size() / 3;

    mesh = {0};
    mesh.vertexCount = static_cast<int>(vertexCount);
    mesh.triangleCount = static_cast<int>(vertexCount / 3);

    mesh.vertices = static_cast<float*>(RL_MALLOC(vertices.size() * sizeof(float)));
    mesh.texcoords = static_cast<float*>(RL_MALLOC(texcoords.size() * sizeof(float)));
    mesh.normals = static_cast<float*>(RL_MALLOC(normals.size() * sizeof(float)));
    mesh.indices = nullptr;

    std::copy(vertices.begin(), vertices.end(), mesh.vertices);
    std::copy(texcoords.begin(), texcoords.end(), mesh.texcoords);
    std::copy(normals.begin(), normals.end(), mesh.normals);
}
}

GameMesh::GameMesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, GameShader* shader)
    : m_Vertices(vertices ? *vertices : std::vector<Vertex>{}), m_Indices(indices ? *indices : std::vector<unsigned int>{}), p_Shader(shader), m_OnGPU(false), m_ModelMatrix(1.0f)
{
}

GameMesh::GameMesh(std::vector<float> vertices, std::vector<float> texcoords, std::vector<float> normals)
    : m_ExpandedVertices(std::move(vertices)), m_ExpandedTexcoords(std::move(texcoords)), m_ExpandedNormals(std::move(normals)), m_OnGPU(false), m_ModelMatrix(1.0f)
{
}

void GameMesh::UpdateBounds()
{
    m_HasBounds = false;
    m_BoundsMin = {0.0f, 0.0f, 0.0f};
    m_BoundsMax = {0.0f, 0.0f, 0.0f};

    auto includePosition = [this](const glm::vec3& position)
    {
        if (!m_HasBounds)
        {
            m_BoundsMin = position;
            m_BoundsMax = position;
            m_HasBounds = true;
            return;
        }

        m_BoundsMin = glm::min(m_BoundsMin, position);
        m_BoundsMax = glm::max(m_BoundsMax, position);
    };

    if (HasExpandedGeometry())
    {
        for (size_t i = 0; i + 2 < m_ExpandedVertices.size(); i += 3)
        {
            includePosition({m_ExpandedVertices[i], m_ExpandedVertices[i + 1], m_ExpandedVertices[i + 2]});
        }
        return;
    }

    for (const unsigned int index : m_Indices)
    {
        if (index < m_Vertices.size())
        {
            includePosition(m_Vertices[index].Position());
        }
    }
}

void GameMesh::Bind()
{
}

void GameMesh::Unbind()
{
}

void GameMesh::Initialize()
{
    UpdateBounds();

    if (HasExpandedGeometry())
    {
        PopulateRaylibMesh(m_RaylibMesh, m_ExpandedVertices, m_ExpandedTexcoords, m_ExpandedNormals);
    }
    else
    {
        PopulateRaylibMesh(m_RaylibMesh, m_Vertices, m_Indices);
    }
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

    UpdateBounds();

    if (HasExpandedGeometry())
    {
        PopulateRaylibMesh(m_RaylibMesh, m_ExpandedVertices, m_ExpandedTexcoords, m_ExpandedNormals);
    }
    else
    {
        PopulateRaylibMesh(m_RaylibMesh, m_Vertices, m_Indices);
    }
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
