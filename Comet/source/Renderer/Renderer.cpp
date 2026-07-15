#include "Renderer.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <array>
#include <raymath.h>
#include <rlgl.h>

RenderLock Renderer::QueueLock;

namespace
{
constexpr int ChunkWidth = 16;
constexpr int BatchWidthChunks = 2;
constexpr int MaxBatchRebuildsPerFrame = 1;

struct Frustum
{
    std::array<glm::vec4, 6> Planes;
};

int FloorDiv(int value, int divisor)
{
    int quotient = value / divisor;
    const int remainder = value % divisor;
    if (remainder != 0 && ((remainder < 0) != (divisor < 0)))
    {
        quotient--;
    }
    return quotient;
}

glm::ivec3 BatchIndexForChunk(const glm::ivec3& chunkIndex)
{
    return {
        FloorDiv(chunkIndex.x, BatchWidthChunks),
        chunkIndex.y,
        FloorDiv(chunkIndex.z, BatchWidthChunks)
    };
}

glm::ivec3 BatchOriginChunk(const glm::ivec3& batchIndex)
{
    return {
        batchIndex.x * BatchWidthChunks,
        batchIndex.y,
        batchIndex.z * BatchWidthChunks
    };
}

glm::vec4 MatrixRow(const glm::mat4& matrix, int row)
{
    return {
        matrix[0][row],
        matrix[1][row],
        matrix[2][row],
        matrix[3][row]
    };
}

glm::vec4 NormalizePlane(const glm::vec4& plane)
{
    const float length = glm::length(glm::vec3(plane));
    if (length <= 0.0f)
    {
        return plane;
    }
    return plane / length;
}

Frustum BuildCameraFrustum()
{
    const glm::mat4 viewProjection = Comet::ViewCamera::ProjMatrix() * Comet::ViewCamera::ViewMatrix();
    const glm::vec4 row0 = MatrixRow(viewProjection, 0);
    const glm::vec4 row1 = MatrixRow(viewProjection, 1);
    const glm::vec4 row2 = MatrixRow(viewProjection, 2);
    const glm::vec4 row3 = MatrixRow(viewProjection, 3);

    return {{
        NormalizePlane(row3 + row0),
        NormalizePlane(row3 - row0),
        NormalizePlane(row3 + row1),
        NormalizePlane(row3 - row1),
        NormalizePlane(row3 + row2),
        NormalizePlane(row3 - row2)
    }};
}

glm::vec3 BatchWorldOffset(const glm::ivec3& batchIndex)
{
    const float batchWidth = static_cast<float>(ChunkWidth * BatchWidthChunks);
    return {
        static_cast<float>(batchIndex.x) * batchWidth,
        0.0f,
        static_cast<float>(batchIndex.z) * batchWidth
    };
}

bool IsMeshVisible(const Frustum& frustum, const glm::ivec3& batchIndex, const GameMesh& mesh)
{
    if (!mesh.HasBounds())
    {
        return false;
    }

    const glm::vec3 batchOffset = BatchWorldOffset(batchIndex);
    const glm::vec3 min = mesh.BoundsMin() + batchOffset;
    const glm::vec3 max = mesh.BoundsMax() + batchOffset;

    for (const glm::vec4& plane : frustum.Planes)
    {
        const glm::vec3 positiveVertex = {
            plane.x >= 0.0f ? max.x : min.x,
            plane.y >= 0.0f ? max.y : min.y,
            plane.z >= 0.0f ? max.z : min.z
        };

        if (glm::dot(glm::vec3(plane), positiveVertex) + plane.w < 0.0f)
        {
            return false;
        }
    }

    return true;
}

void AppendChunkMeshToBatch(
    const glm::ivec3& chunkIndex,
    const glm::ivec3& batchIndex,
    const GameMesh& chunkMesh,
    std::vector<float>& vertices,
    std::vector<float>& texcoords,
    std::vector<float>& normals)
{
    const glm::ivec3 batchOrigin = BatchOriginChunk(batchIndex);
    const glm::vec3 chunkOffset = {
        static_cast<float>((chunkIndex.x - batchOrigin.x) * ChunkWidth),
        0.0f,
        static_cast<float>((chunkIndex.z - batchOrigin.z) * ChunkWidth)
    };

    const std::vector<unsigned int>& indices = chunkMesh.GetIndices();
    vertices.reserve(vertices.size() + indices.size() * 3);
    texcoords.reserve(texcoords.size() + indices.size() * 2);
    normals.reserve(normals.size() + indices.size() * 3);

    for (const unsigned int index : indices)
    {
        const Vertex& vertex = chunkMesh.GetVertices().at(index);
        const glm::vec3 position = vertex.Position() + chunkOffset;
        const glm::vec2 textureCoordinate = vertex.TextureCoordinate();
        const glm::vec3 normal = vertex.Normal();

        vertices.push_back(position.x);
        vertices.push_back(position.y);
        vertices.push_back(position.z);

        texcoords.push_back(textureCoordinate.x);
        texcoords.push_back(textureCoordinate.y);

        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    }
}

void RebuildBatchMesh(
    const glm::ivec3& batchIndex,
    const std::unordered_map<glm::ivec3, GameMesh>& chunkMeshes,
    std::unordered_map<glm::ivec3, GameMesh>& batchMeshes)
{
    std::vector<float> vertices;
    std::vector<float> texcoords;
    std::vector<float> normals;

    for (const auto& [chunkIndex, chunkMesh] : chunkMeshes)
    {
        if (BatchIndexForChunk(chunkIndex) == batchIndex)
        {
            AppendChunkMeshToBatch(chunkIndex, batchIndex, chunkMesh, vertices, texcoords, normals);
        }
    }

    if (auto entry = batchMeshes.find(batchIndex); entry != batchMeshes.end())
    {
        entry->second.Finalize();
        batchMeshes.erase(entry);
    }

    if (vertices.empty())
    {
        return;
    }

    GameMesh batchMesh(std::move(vertices), std::move(texcoords), std::move(normals));
    batchMeshes.insert_or_assign(batchIndex, batchMesh);
    batchMeshes.at(batchIndex).Initialize();
}
}

void Renderer::Initialize()
{
    Get();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplOpenGL3_Init("#version 330");
}

void Renderer::Finalize()
{
    auto& renderer = Get();

    for (auto& [index, mesh] : renderer.m_MeshMap)
    {
        mesh.Finalize();
    }
    renderer.m_MeshMap.clear();
    for (auto& [index, mesh] : renderer.m_WaterMeshMap)
    {
        mesh.Finalize();
    }
    renderer.m_WaterMeshMap.clear();
    for (auto& [index, mesh] : renderer.m_BatchedMeshMap)
    {
        mesh.Finalize();
    }
    renderer.m_BatchedMeshMap.clear();
    for (auto& [index, mesh] : renderer.m_BatchedWaterMeshMap)
    {
        mesh.Finalize();
    }
    renderer.m_BatchedWaterMeshMap.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::NewFrame()
{
    BeginDrawing();
    ClearBackground(Color{
        static_cast<unsigned char>(Get().m_BackgroundColor.x * 255),
        static_cast<unsigned char>(Get().m_BackgroundColor.y * 255),
        static_cast<unsigned char>(Get().m_BackgroundColor.z * 255),
        255
    });
}

void Renderer::DrawMeshQueue()
{
    size_t drawCallCount = 0;
    ProcessMeshQueues();

    Comet::ViewCamera::Update();
    const Frustum cameraFrustum = BuildCameraFrustum();
    ::Camera3D raylibCam = Comet::ViewCamera::GetRaylibCamera();
    BeginMode3D(raylibCam);
    rlDisableBackfaceCulling();

    if (Get().m_BlockMaterial.shader.id > 0)
    {
        if (Get().m_WireMeshEnabled)
        {
            rlEnableWireMode();
        }

        auto drawChunkMesh = [&drawCallCount, &cameraFrustum](const glm::ivec3& index, GameMesh& mesh)
        {
            if (!IsMeshVisible(cameraFrustum, index, mesh))
            {
                return;
            }

            mesh.Update();

            ::Shader rShader = Get().m_BlockMaterial.shader;
            int locOverlay = GetShaderLocation(rShader, "u_OverlayColor");
            if (locOverlay >= 0)
            {
                SetShaderValue(rShader, locOverlay, &Get().m_OverlayColor, SHADER_UNIFORM_VEC3);
            }

            Matrix transform = MatrixTranslate(
                BatchWorldOffset(index).x,
                BatchWorldOffset(index).y,
                BatchWorldOffset(index).z
            );

            DrawMesh(mesh.GetRaylibMesh(), Get().m_BlockMaterial, transform);
            drawCallCount++;
        };

        for (auto& [index, mesh] : Get().m_BatchedMeshMap)
        {
            drawChunkMesh(index, mesh);
        }

        rlDrawRenderBatchActive();
        rlDisableDepthMask();
        BeginBlendMode(BLEND_ALPHA);
        for (auto& [index, mesh] : Get().m_BatchedWaterMeshMap)
        {
            drawChunkMesh(index, mesh);
        }
        EndBlendMode();
        rlEnableDepthMask();

        if (Get().m_WireMeshEnabled)
        {
            rlDisableWireMode();
        }
    }

    EndMode3D();

    SetDrawCallsPerFrame(drawCallCount);
}

void Renderer::DrawInterfaceQueue()
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()));
    io.DeltaTime = GetFrameTime();

    io.MousePos = ImVec2(static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY()));
    io.MouseDown[0] = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    io.MouseDown[1] = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    io.MouseDown[2] = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);

    Vector2 wheelMove = GetMouseWheelMoveV();
    io.MouseWheel = wheelMove.y;
    io.MouseWheelH = wheelMove.x;

    for (int key = 0; key < 512; key++)
    {
        io.KeysDown[key] = IsKeyDown(key);
    }

    io.KeyCtrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    io.KeyShift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    io.KeyAlt = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    io.KeySuper = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);

    int key = GetCharPressed();
    while (key > 0)
    {
        io.AddInputCharacter(static_cast<unsigned int>(key));
        key = GetCharPressed();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    LayerManager::Draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Renderer::AddMeshToQueue(glm::ivec3 index, const GameMesh& mesh)
{
    std::lock_guard lock(QueueLock.AddQueue);
    Get().m_MeshesToAdd.insert_or_assign(index, mesh);
}

void Renderer::AddWaterMeshToQueue(glm::ivec3 index, const GameMesh& mesh)
{
    std::lock_guard lock(QueueLock.AddQueue);
    Get().m_WaterMeshesToAdd.insert_or_assign(index, mesh);
}

void Renderer::UpdateMeshInQueue(glm::ivec3 index)
{
    std::lock_guard lock(QueueLock.UpdateQueue);
    Get().m_MeshesToUpdate.insert(index);
}

void Renderer::DeleteMeshFromQueue(glm::ivec3 index)
{
    std::lock_guard lock(QueueLock.DeleteQueue);
    Get().m_MeshesToDelete.insert(index);
}

void Renderer::ProcessMeshQueues()
{
    {
        std::lock_guard lock(QueueLock.AddQueue);
        for (const auto& [index, mesh] : Get().m_MeshesToAdd)
        {
            Get().m_MeshMap.insert_or_assign(index, mesh);
            Get().m_BatchesToUpdate.insert(BatchIndexForChunk(index));
        }
        Get().m_MeshesToAdd.clear();
        for (const auto& [index, mesh] : Get().m_WaterMeshesToAdd)
        {
            if (mesh.GetIndices().empty())
            {
                if (auto entry = Get().m_WaterMeshMap.find(index); entry != Get().m_WaterMeshMap.end())
                {
                    Get().m_WaterMeshMap.erase(entry);
                    Get().m_BatchesToUpdate.insert(BatchIndexForChunk(index));
                }
                continue;
            }

            Get().m_WaterMeshMap.insert_or_assign(index, mesh);
            Get().m_BatchesToUpdate.insert(BatchIndexForChunk(index));
        }
        Get().m_WaterMeshesToAdd.clear();
    }

    {
        std::lock_guard lock(QueueLock.UpdateQueue);
        for (const auto& index : Get().m_MeshesToUpdate)
        {
            if (auto entry = Get().m_MeshMap.find(index); entry != Get().m_MeshMap.end())
            {
                Get().m_BatchesToUpdate.insert(BatchIndexForChunk(index));
            }
            if (auto entry = Get().m_WaterMeshMap.find(index); entry != Get().m_WaterMeshMap.end())
            {
                Get().m_BatchesToUpdate.insert(BatchIndexForChunk(index));
            }
        }
        Get().m_MeshesToUpdate.clear();
    } 

    {
        std::lock_guard lock(QueueLock.DeleteQueue);
        for (const auto& index : Get().m_MeshesToDelete)
        {
            if (auto entry = Get().m_MeshMap.find(index); entry != Get().m_MeshMap.end())
            {
                Get().m_MeshMap.erase(index);
                Get().m_BatchesToUpdate.insert(BatchIndexForChunk(index));
            }
            if (auto entry = Get().m_WaterMeshMap.find(index); entry != Get().m_WaterMeshMap.end())
            {
                Get().m_WaterMeshMap.erase(index);
                Get().m_BatchesToUpdate.insert(BatchIndexForChunk(index));
            }
        }
        Get().m_MeshesToDelete.clear();
    }

    int rebuiltBatchCount = 0;
    for (auto batchIt = Get().m_BatchesToUpdate.begin(); batchIt != Get().m_BatchesToUpdate.end() && rebuiltBatchCount < MaxBatchRebuildsPerFrame;)
    {
        const glm::ivec3 batchIndex = *batchIt;
        RebuildBatchMesh(batchIndex, Get().m_MeshMap, Get().m_BatchedMeshMap);
        RebuildBatchMesh(batchIndex, Get().m_WaterMeshMap, Get().m_BatchedWaterMeshMap);
        batchIt = Get().m_BatchesToUpdate.erase(batchIt);
        rebuiltBatchCount++;
    }
}

void Renderer::Update()
{
    NewFrame();
    DrawMeshQueue();
    rlDrawRenderBatchActive();
    DrawInterfaceQueue();
    EndDrawing();
}
