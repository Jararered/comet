#pragma once

#include "Layer/LayerManager.h"

#include "Mesh.h"
#include "Shader.h"

#include "ViewCamera.h"

#include <glm/gtx/hash.hpp>

#include <mutex>

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
    inline static auto& Get()
    {
        static Renderer instance;
        return instance;
    }

    static RenderLock QueueLock;

    static void Initialize();
    static void Finalize();
    static void Update();

    static void NewFrame();
    static void DrawMeshQueue();
    static void DrawInterfaceQueue();

    static void AddMeshToQueue(glm::ivec3 index, const GameMesh& mesh);
    static void AddWaterMeshToQueue(glm::ivec3 index, const GameMesh& mesh);
    static void UpdateMeshInQueue(glm::ivec3 index);
    static void DeleteMeshFromQueue(glm::ivec3 index);
    static void ProcessMeshQueues();

    static void SetBlockMaterial(const ::Material& mat) { Get().m_BlockMaterial = mat; }
    static ::Material GetBlockMaterial() { return Get().m_BlockMaterial; }

private:
    Renderer() {}
    Renderer(Renderer const&);
    void operator=(Renderer const&);

    unsigned int m_DrawCallsPerFrame = 0;

    glm::vec3 m_OverlayColor = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_BackgroundColor = {0.529f, 0.808f, 0.980f};

    std::unordered_map<glm::ivec3, GameMesh> m_MeshMap;
    std::unordered_map<glm::ivec3, GameMesh> m_WaterMeshMap;

    std::unordered_map<glm::ivec3, GameMesh> m_MeshesToAdd;
    std::unordered_map<glm::ivec3, GameMesh> m_WaterMeshesToAdd;
    std::unordered_set<glm::ivec3> m_MeshesToUpdate;
    std::unordered_set<glm::ivec3> m_MeshesToDelete;

    ::Material m_BlockMaterial = {0};

public:
    static glm::vec3 OverlayColor() { return Get().m_OverlayColor; }
    static void SetOverlayColor(const glm::vec3& OverlayColor) { Get().m_OverlayColor = OverlayColor; }

    static glm::vec3 BackgroundColor() { return Get().m_BackgroundColor; }
    static void SetBackgroundColor(const glm::vec3& BackgroundColor) { Get().m_BackgroundColor = BackgroundColor; }

    static unsigned int DrawCallsPerFrame() { return Get().m_DrawCallsPerFrame; }
    static void SetDrawCallsPerFrame(unsigned int DrawCallsPerFrame) { Get().m_DrawCallsPerFrame = DrawCallsPerFrame; }
};
