#include "Renderer.h"
#include "../Profiler/Profiler.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <optional>
#include <raymath.h>
#include <rlgl.h>
#include <utility>
#include <vector>

namespace
{
    constexpr int ChunkWidth = 16;
    constexpr auto ChunkUploadTimeBudget = std::chrono::microseconds(750);
    constexpr std::size_t ChunkUploadByteBudget = 4 * 1024 * 1024;

    struct Frustum
    {
        std::array<glm::vec4, 6> Planes;
    };

    glm::vec4 MatrixRow(const glm::mat4& matrix, int row)
    {
        return {matrix[0][row], matrix[1][row], matrix[2][row], matrix[3][row]};
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

    Frustum BuildCameraFrustum(const Comet::ViewCamera& camera)
    {
        const glm::mat4 viewProjection = camera.ProjMatrix() * camera.ViewMatrix();
        const glm::vec4 row0 = MatrixRow(viewProjection, 0);
        const glm::vec4 row1 = MatrixRow(viewProjection, 1);
        const glm::vec4 row2 = MatrixRow(viewProjection, 2);
        const glm::vec4 row3 = MatrixRow(viewProjection, 3);

        return {{NormalizePlane(row3 + row0), NormalizePlane(row3 - row0), NormalizePlane(row3 + row1), NormalizePlane(row3 - row1), NormalizePlane(row3 + row2), NormalizePlane(row3 - row2)}};
    }

    glm::vec3 ChunkWorldOffset(const glm::ivec3& chunkIndex)
    {
        return {static_cast<float>(chunkIndex.x * ChunkWidth), 0.0f, static_cast<float>(chunkIndex.z * ChunkWidth)};
    }

    bool IsMeshVisible(const Frustum& frustum, const glm::ivec3& chunkIndex, const GameMesh& mesh)
    {
        if (!mesh.HasBounds())
        {
            return false;
        }

        const glm::vec3 chunkOffset = ChunkWorldOffset(chunkIndex);
        const glm::vec3 min = mesh.BoundsMin() + chunkOffset;
        const glm::vec3 max = mesh.BoundsMax() + chunkOffset;

        for (const glm::vec4& plane : frustum.Planes)
        {
            const glm::vec3 positiveVertex = {plane.x >= 0.0f ? max.x : min.x, plane.y >= 0.0f ? max.y : min.y, plane.z >= 0.0f ? max.z : min.z};

            if (glm::dot(glm::vec3(plane), positiveVertex) + plane.w < 0.0f)
            {
                return false;
            }
        }

        return true;
    }

    void ReplaceMesh(std::unordered_map<glm::ivec3, GameMesh>& meshes, const glm::ivec3& index, GameMesh&& mesh)
    {
        if (mesh.Empty())
        {
            if (auto entry = meshes.find(index); entry != meshes.end())
            {
                entry->second.Finalize();
                meshes.erase(entry);
            }
            return;
        }

        mesh.Initialize();

        if (auto entry = meshes.find(index); entry != meshes.end())
        {
            // Keep the current GPU mesh alive until its replacement has finished
            // uploading, then transfer ownership and release the old resources.
            entry->second = std::move(mesh);
        }
        else
        {
            meshes.emplace(index, std::move(mesh));
        }
    }
}

void Renderer::Initialize()
{
    m_DefaultCamera.Initialize();
}

void Renderer::Finalize()
{
    for (auto& [index, mesh] : m_MeshMap)
    {
        mesh.Finalize();
    }
    m_MeshMap.clear();
    for (auto& [index, mesh] : m_WaterMeshMap)
    {
        mesh.Finalize();
    }
    m_WaterMeshMap.clear();

    std::lock_guard lock(m_QueueLock.AddQueue);
    m_MeshesToAdd.clear();
    m_MeshesToAddOrder.clear();
    m_QueuedUploadBytes = 0;
}

void Renderer::SetBlockMaterial(const ::Material& mat)
{
    m_BlockMaterial = mat;
    m_OverlayColorLocation = -1;

    if (m_BlockMaterial.shader.id > 0)
    {
        m_OverlayColorLocation = GetShaderLocation(m_BlockMaterial.shader, "u_OverlayColor");
    }
}

void Renderer::SetBlockOverlay(glm::ivec3 blockPosition)
{
    m_BlockOverlayPosition = blockPosition;
    m_BlockOverlayVisible = true;
}

void Renderer::ClearBlockOverlay()
{
    m_BlockOverlayVisible = false;
}

void Renderer::NewFrame()
{
    BeginDrawing();
    ClearBackground(Color{static_cast<unsigned char>(m_BackgroundColor.x * 255), static_cast<unsigned char>(m_BackgroundColor.y * 255), static_cast<unsigned char>(m_BackgroundColor.z * 255), 255});
}

void Renderer::DrawMeshQueue(Comet::ViewCamera& camera)
{
    COMET_PROFILE_SCOPE("Renderer::DrawMeshQueue", "render");
    size_t drawCallCount = 0;
    ProcessMeshQueues();

    camera.Update();
    const Frustum cameraFrustum = BuildCameraFrustum(camera);
    ::Camera3D raylibCam = camera.GetRaylibCamera();
    BeginMode3D(raylibCam);
    rlDisableBackfaceCulling();

    if (m_BlockMaterial.shader.id > 0)
    {
        if (m_WireMeshEnabled)
        {
            rlEnableWireMode();
        }

        ::Shader rShader = m_BlockMaterial.shader;
        if (m_OverlayColorLocation >= 0)
        {
            SetShaderValue(rShader, m_OverlayColorLocation, &m_OverlayColor, SHADER_UNIFORM_VEC3);
        }

        auto drawVisibleChunk = [this, &drawCallCount](const glm::ivec3& index, GameMesh& mesh)
        {
            mesh.Update();

            Matrix transform = MatrixTranslate(ChunkWorldOffset(index).x, ChunkWorldOffset(index).y, ChunkWorldOffset(index).z);

            DrawMesh(mesh.GetRaylibMesh(), m_BlockMaterial, transform);
            drawCallCount++;
        };

        auto drawVisibleChunkMesh = [&cameraFrustum, &drawVisibleChunk](const glm::ivec3& index, GameMesh& mesh)
        {
            if (!IsMeshVisible(cameraFrustum, index, mesh))
            {
                return false;
            }

            drawVisibleChunk(index, mesh);
            return true;
        };

        for (auto& [index, mesh] : m_MeshMap)
        {
            drawVisibleChunkMesh(index, mesh);
        }

        if (m_BlockOverlayVisible)
        {
            rlDrawRenderBatchActive();
            BeginBlendMode(BLEND_ALPHA);
            const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(GetTime()) * 5.0f);
            const float fillSize = 1.015f + pulse * 0.025f;
            const float wireSize = fillSize + 0.01f;
            const unsigned char fillAlpha = static_cast<unsigned char>(36.0f + pulse * 24.0f);
            const unsigned char wireAlpha = static_cast<unsigned char>(150.0f + pulse * 70.0f);
            const Vector3 position = {static_cast<float>(m_BlockOverlayPosition.x), static_cast<float>(m_BlockOverlayPosition.y), static_cast<float>(m_BlockOverlayPosition.z)};
            DrawCube(position, fillSize, fillSize, fillSize, Color{255, 255, 255, fillAlpha});
            DrawCubeWires(position, wireSize, wireSize, wireSize, Color{255, 255, 255, wireAlpha});
            EndBlendMode();
        }

        std::vector<std::pair<glm::ivec3, GameMesh*>> visibleWaterBatches;
        visibleWaterBatches.reserve(m_WaterMeshMap.size());
        for (auto& [index, mesh] : m_WaterMeshMap)
        {
            if (IsMeshVisible(cameraFrustum, index, mesh))
            {
                visibleWaterBatches.emplace_back(index, &mesh);
            }
        }

        if (!visibleWaterBatches.empty())
        {
            rlDrawRenderBatchActive();
            rlDisableDepthMask();
            BeginBlendMode(BLEND_ALPHA);
            for (auto& [index, mesh] : visibleWaterBatches)
            {
                drawVisibleChunk(index, *mesh);
            }
            EndBlendMode();
            rlEnableDepthMask();
        }

        if (m_WireMeshEnabled)
        {
            rlDisableWireMode();
        }
    }

    EndMode3D();

    SetDrawCallsPerFrame(drawCallCount);
}

void Renderer::DrawInterfaceQueue(LayerManager& layerManager)
{
    layerManager.Draw();
}

void Renderer::AddChunkMeshToQueue(glm::ivec3 index, GameMesh terrainMesh, GameMesh waterMesh, bool prioritize)
{
    ChunkMeshUpload upload(std::move(terrainMesh), std::move(waterMesh));
    const std::size_t uploadBytes = upload.CpuByteSize();

    std::lock_guard lock(m_QueueLock.AddQueue);
    if (auto entry = m_MeshesToAdd.find(index); entry != m_MeshesToAdd.end())
    {
        m_QueuedUploadBytes -= entry->second.CpuByteSize();
        entry->second = std::move(upload);
    }
    else
    {
        m_MeshesToAdd.emplace(index, std::move(upload));
        m_MeshesToAddOrder.push_back(index);
    }
    m_QueuedUploadBytes += uploadBytes;

    if (prioritize)
    {
        std::erase(m_MeshesToAddOrder, index);
        m_MeshesToAddOrder.insert(m_MeshesToAddOrder.begin(), index);
    }
}

void Renderer::DeleteMeshFromQueue(glm::ivec3 index)
{
    std::scoped_lock lock(m_QueueLock.AddQueue, m_QueueLock.DeleteQueue);
    if (auto entry = m_MeshesToAdd.find(index); entry != m_MeshesToAdd.end())
    {
        m_QueuedUploadBytes -= entry->second.CpuByteSize();
        m_MeshesToAdd.erase(entry);
    }
    std::erase(m_MeshesToAddOrder, index);
    m_MeshesToDelete.insert(index);
}

void Renderer::ProcessMeshQueues()
{
    COMET_PROFILE_SCOPE("Renderer::ProcessMeshQueues", "mesh_upload");
    const auto uploadStart = std::chrono::steady_clock::now();
    std::size_t uploadedBytes = 0;
    std::uint64_t uploadedChunks = 0;

    while (true)
    {
        std::optional<std::pair<glm::ivec3, ChunkMeshUpload>> pendingUpload;
        {
            std::lock_guard lock(m_QueueLock.AddQueue);

            while (!m_MeshesToAddOrder.empty())
            {
                const glm::ivec3 index = m_MeshesToAddOrder.front();
                const auto meshEntry = m_MeshesToAdd.find(index);
                if (meshEntry == m_MeshesToAdd.end())
                {
                    m_MeshesToAddOrder.erase(m_MeshesToAddOrder.begin());
                    continue;
                }

                const std::size_t uploadBytes = meshEntry->second.CpuByteSize();
                pendingUpload.emplace(index, std::move(meshEntry->second));
                m_QueuedUploadBytes -= uploadBytes;
                m_MeshesToAdd.erase(meshEntry);
                m_MeshesToAddOrder.erase(m_MeshesToAddOrder.begin());
                break;
            }
        }

        if (!pendingUpload)
        {
            break;
        }

        const glm::ivec3 index = pendingUpload->first;
        uploadedBytes += pendingUpload->second.UploadByteSize();
        ReplaceMesh(m_MeshMap, index, std::move(pendingUpload->second.Terrain));
        ReplaceMesh(m_WaterMeshMap, index, std::move(pendingUpload->second.Water));
        uploadedChunks++;

        if (uploadedBytes >= ChunkUploadByteBudget || std::chrono::steady_clock::now() - uploadStart >= ChunkUploadTimeBudget)
        {
            break;
        }
    }

    std::size_t queuedUploadBytes = 0;
    std::size_t queuedUploadCount = 0;
    {
        std::lock_guard lock(m_QueueLock.AddQueue);
        queuedUploadBytes = m_QueuedUploadBytes;
        queuedUploadCount = m_MeshesToAdd.size();
    }

    if (uploadedChunks > 0 || queuedUploadCount > 0)
    {
        Comet::Profiler::Instance().Record("Renderer::UploadedChunks", "mesh_upload_metrics", 0.0, uploadedChunks);
        Comet::Profiler::Instance().Record("Renderer::UploadedBytes", "mesh_upload_metrics", 0.0, uploadedBytes);
        Comet::Profiler::Instance().Record("Renderer::QueuedChunks", "mesh_upload_metrics", 0.0, queuedUploadCount);
        Comet::Profiler::Instance().Record("Renderer::QueuedBytes", "mesh_upload_metrics", 0.0, queuedUploadBytes);
    }

    {
        std::lock_guard lock(m_QueueLock.DeleteQueue);
        for (const auto& index : m_MeshesToDelete)
        {
            if (auto entry = m_MeshMap.find(index); entry != m_MeshMap.end())
            {
                entry->second.Finalize();
                m_MeshMap.erase(entry);
            }
            if (auto entry = m_WaterMeshMap.find(index); entry != m_WaterMeshMap.end())
            {
                entry->second.Finalize();
                m_WaterMeshMap.erase(entry);
            }
        }
        m_MeshesToDelete.clear();
    }
}

void Renderer::Update(LayerManager& layerManager)
{
    COMET_PROFILE_SCOPE("Renderer::Update", "render");
    const std::shared_ptr<Comet::ViewCamera> sharedCamera = m_Camera.lock();
    Comet::ViewCamera& camera = sharedCamera ? *sharedCamera : m_DefaultCamera;
    NewFrame();
    DrawMeshQueue(camera);
    rlDrawRenderBatchActive();
    DrawInterfaceQueue(layerManager);
    EndDrawing();
}
