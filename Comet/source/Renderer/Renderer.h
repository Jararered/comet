#pragma once

#include "Layer/LayerManager.h"

#include "Mesh.h"
#include "Shader.h"

#include "ViewCamera.h"

#include <glm/gtx/hash.hpp>

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <raylib.h>

struct RenderLock
{
    std::mutex AddQueue;
    std::mutex UpdateQueue;
    std::mutex DeleteQueue;
};

class Renderer
{
public:
    Renderer() = default;

    void Initialize();
    void Finalize();
    void Update(LayerManager& layerManager, Comet::ViewCamera& camera);

    void NewFrame();
    void DrawMeshQueue(Comet::ViewCamera& camera);
    void DrawInterfaceQueue(LayerManager& layerManager);

    void AddMeshToQueue(glm::ivec3 index, const GameMesh& mesh);
    void AddPriorityMeshToQueue(glm::ivec3 index, const GameMesh& mesh);
    void AddWaterMeshToQueue(glm::ivec3 index, const GameMesh& mesh);
    void UpdateMeshInQueue(glm::ivec3 index);
    void DeleteMeshFromQueue(glm::ivec3 index);
    void ProcessMeshQueues();

    void SetBlockMaterial(const ::Material& mat);
    ::Material GetBlockMaterial() const { return m_BlockMaterial; }

private:
    RenderLock m_QueueLock;

    unsigned int m_DrawCallsPerFrame = 0;

    glm::vec3 m_OverlayColor = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_BackgroundColor = {0.529f, 0.808f, 0.980f};
    bool m_WireMeshEnabled = false;

    std::unordered_map<glm::ivec3, GameMesh> m_MeshMap;
    std::unordered_map<glm::ivec3, GameMesh> m_WaterMeshMap;
    std::unordered_map<glm::ivec3, GameMesh> m_BatchedMeshMap;
    std::unordered_map<glm::ivec3, GameMesh> m_BatchedWaterMeshMap;

    std::unordered_map<glm::ivec3, GameMesh> m_MeshesToAdd;
    std::unordered_map<glm::ivec3, GameMesh> m_WaterMeshesToAdd;
    std::vector<glm::ivec3> m_MeshesToAddOrder;
    std::unordered_set<glm::ivec3> m_MeshesToUpdate;
    std::unordered_set<glm::ivec3> m_MeshesToDelete;
    std::unordered_set<glm::ivec3> m_MeshBatchesToUpdate;
    std::unordered_set<glm::ivec3> m_WaterBatchesToUpdate;
    int m_ChunkRenderFramesUntilNextAdd = 0;

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
