#pragma once

#include "Layer/LayerManager.h"

#include "Mesh.h"
#include "Shader.h"

#include "ViewCamera.h"

#include <glm/gtx/hash.hpp>

#include <cstddef>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <raylib.h>

struct RenderLock
{
    std::mutex AddQueue;
    std::mutex DeleteQueue;
};

class Renderer
{
public:
    Renderer() = default;

    void Initialize();
    void Finalize();
    void Update(LayerManager& layerManager);
    void SetCamera(const std::shared_ptr<Comet::ViewCamera>& camera) { m_Camera = camera; }

    void NewFrame();
    void DrawMeshQueue(Comet::ViewCamera& camera);
    void DrawInterfaceQueue(LayerManager& layerManager);

    void AddChunkMeshToQueue(glm::ivec3 index, GameMesh terrainMesh, GameMesh waterMesh, bool prioritize = false);
    void DeleteMeshFromQueue(glm::ivec3 index);
    void ProcessMeshQueues();

    void SetBlockMaterial(const ::Material& mat);
    ::Material GetBlockMaterial() const { return m_BlockMaterial; }
    void SetBlockOverlay(glm::ivec3 blockPosition);
    void ClearBlockOverlay();

private:
    struct ChunkMeshUpload
    {
        GameMesh Terrain;
        GameMesh Water;

        ChunkMeshUpload(GameMesh terrain, GameMesh water) : Terrain(std::move(terrain)), Water(std::move(water)) {}
        std::size_t CpuByteSize() const { return Terrain.CpuByteSize() + Water.CpuByteSize(); }
        std::size_t UploadByteSize() const { return Terrain.UploadByteSize() + Water.UploadByteSize(); }
    };

    std::weak_ptr<Comet::ViewCamera> m_Camera;
    Comet::ViewCamera m_DefaultCamera;
    RenderLock m_QueueLock;

    unsigned int m_DrawCallsPerFrame = 0;

    glm::vec3 m_OverlayColor = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_BackgroundColor = {0.529f, 0.808f, 0.980f};
    bool m_WireMeshEnabled = false;
    bool m_BlockOverlayVisible = false;
    glm::ivec3 m_BlockOverlayPosition = {0, 0, 0};

    std::unordered_map<glm::ivec3, GameMesh> m_MeshMap;
    std::unordered_map<glm::ivec3, GameMesh> m_WaterMeshMap;

    std::unordered_map<glm::ivec3, ChunkMeshUpload> m_MeshesToAdd;
    std::vector<glm::ivec3> m_MeshesToAddOrder;
    std::unordered_set<glm::ivec3> m_MeshesToDelete;
    std::size_t m_QueuedUploadBytes = 0;

    ::Material m_BlockMaterial = {0};
    int m_OverlayColorLocation = -1;

public:
    glm::vec3 OverlayColor() const { return m_OverlayColor; }
    void SetOverlayColor(const glm::vec3& OverlayColor) { m_OverlayColor = OverlayColor; }

    glm::vec3 BackgroundColor() const { return m_BackgroundColor; }
    void SetBackgroundColor(const glm::vec3& BackgroundColor) { m_BackgroundColor = BackgroundColor; }

    bool WireMeshEnabled() const { return m_WireMeshEnabled; }
    void SetWireMeshEnabled(bool enabled) { m_WireMeshEnabled = enabled; }

    unsigned int DrawCallsPerFrame() const { return m_DrawCallsPerFrame; }
    void SetDrawCallsPerFrame(unsigned int DrawCallsPerFrame) { m_DrawCallsPerFrame = DrawCallsPerFrame; }
};
