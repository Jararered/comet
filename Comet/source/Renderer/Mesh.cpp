#include "Mesh.h"

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <limits>
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
        constexpr std::size_t MaxIndexedVertexCount = static_cast<std::size_t>(std::numeric_limits<unsigned short>::max()) + 1;
        const bool hasValidIndices = std::ranges::all_of(indices, [&vertices](unsigned int index) { return index < vertices.size(); });
        const bool canUseIndices =
            hasValidIndices && vertices.size() <= MaxIndexedVertexCount && std::ranges::all_of(indices, [](unsigned int index) { return index <= std::numeric_limits<unsigned short>::max(); });
        const size_t vertexCount = canUseIndices ? vertices.size() : indices.size();

        if (!hasValidIndices)
        {
            mesh = {0};
            return;
        }

        mesh = {0};
        mesh.vertexCount = static_cast<int>(vertexCount);
        mesh.triangleCount = static_cast<int>(indices.size() / 3);

        mesh.vertices = static_cast<float*>(RL_MALLOC(vertexCount * 3 * sizeof(float)));
        mesh.texcoords = static_cast<float*>(RL_MALLOC(vertexCount * 2 * sizeof(float)));
        mesh.normals = static_cast<float*>(RL_MALLOC(vertexCount * 3 * sizeof(float)));
        mesh.colors = static_cast<unsigned char*>(RL_MALLOC(vertexCount * 4 * sizeof(unsigned char)));
        mesh.indices = canUseIndices ? static_cast<unsigned short*>(RL_MALLOC(indices.size() * sizeof(unsigned short))) : nullptr;

        for (size_t i = 0; i < vertexCount; i++)
        {
            const Vertex& vertex = vertices[canUseIndices ? i : indices[i]];
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

        if (canUseIndices)
        {
            std::transform(indices.begin(), indices.end(), mesh.indices, [](unsigned int index) { return static_cast<unsigned short>(index); });
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

GameMesh::GameMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, GameShader* shader)
    : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), p_Shader(shader), m_OnGPU(false), m_ModelMatrix(1.0f)
{
}

GameMesh::GameMesh(std::vector<float> vertices, std::vector<float> texcoords, std::vector<float> normals)
    : m_ExpandedVertices(std::move(vertices)), m_ExpandedTexcoords(std::move(texcoords)), m_ExpandedNormals(std::move(normals)), m_OnGPU(false), m_ModelMatrix(1.0f)
{
}

GameMesh::~GameMesh()
{
    Finalize();
}

GameMesh::GameMesh(GameMesh&& other) noexcept
    : m_Vertices(std::move(other.m_Vertices)), m_Indices(std::move(other.m_Indices)), m_ExpandedVertices(std::move(other.m_ExpandedVertices)),
      m_ExpandedTexcoords(std::move(other.m_ExpandedTexcoords)), m_ExpandedNormals(std::move(other.m_ExpandedNormals)), p_Shader(other.p_Shader), m_RaylibMesh(other.m_RaylibMesh),
      m_OnGPU(other.m_OnGPU), m_HasBounds(other.m_HasBounds), m_BoundsMin(other.m_BoundsMin), m_BoundsMax(other.m_BoundsMax), m_ModelMatrix(other.m_ModelMatrix)
{
    other.m_RaylibMesh = {0};
    other.m_OnGPU = false;
}

GameMesh& GameMesh::operator=(GameMesh&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Finalize();
    m_Vertices = std::move(other.m_Vertices);
    m_Indices = std::move(other.m_Indices);
    m_ExpandedVertices = std::move(other.m_ExpandedVertices);
    m_ExpandedTexcoords = std::move(other.m_ExpandedTexcoords);
    m_ExpandedNormals = std::move(other.m_ExpandedNormals);
    p_Shader = other.p_Shader;
    m_RaylibMesh = other.m_RaylibMesh;
    m_OnGPU = other.m_OnGPU;
    m_HasBounds = other.m_HasBounds;
    m_BoundsMin = other.m_BoundsMin;
    m_BoundsMax = other.m_BoundsMax;
    m_ModelMatrix = other.m_ModelMatrix;

    other.m_RaylibMesh = {0};
    other.m_OnGPU = false;
    return *this;
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
    std::vector<Vertex>().swap(m_Vertices);
    std::vector<unsigned int>().swap(m_Indices);
    std::vector<float>().swap(m_ExpandedVertices);
    std::vector<float>().swap(m_ExpandedTexcoords);
    std::vector<float>().swap(m_ExpandedNormals);
    m_OnGPU = true;
}

void GameMesh::Update()
{
}

std::size_t GameMesh::CpuByteSize() const
{
    return m_Vertices.size() * sizeof(Vertex) + m_Indices.size() * sizeof(unsigned int) + (m_ExpandedVertices.size() + m_ExpandedTexcoords.size() + m_ExpandedNormals.size()) * sizeof(float);
}

std::size_t GameMesh::UploadByteSize() const
{
    if (HasExpandedGeometry())
    {
        return (m_ExpandedVertices.size() + m_ExpandedTexcoords.size() + m_ExpandedNormals.size()) * sizeof(float);
    }

    constexpr std::size_t MaxIndexedVertexCount = static_cast<std::size_t>(std::numeric_limits<unsigned short>::max()) + 1;
    constexpr std::size_t ExpandedVertexSize = (3 + 2 + 3) * sizeof(float) + 4 * sizeof(unsigned char);
    const bool canUseIndices = m_Vertices.size() <= MaxIndexedVertexCount &&
                               std::ranges::all_of(m_Indices, [this](unsigned int index) { return index < m_Vertices.size() && index <= std::numeric_limits<unsigned short>::max(); });

    if (canUseIndices)
    {
        return m_Vertices.size() * ExpandedVertexSize + m_Indices.size() * sizeof(unsigned short);
    }
    return m_Indices.size() * ExpandedVertexSize;
}

void GameMesh::Finalize()
{
    if (m_OnGPU)
    {
        UnloadMesh(m_RaylibMesh);
        m_RaylibMesh = {0};
        m_OnGPU = false;
    }
}
