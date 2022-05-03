#pragma once

#include <comet.pch>

#include "render/Mesh.h"
#include "render/ShaderProgram.h"

class WindowHandler;
class InterfaceHandler;
class Camera;

struct Lock
{
    std::mutex AddQueue;
    std::mutex UpdateQueue;
    std::mutex DeleteQueue;
};

class Renderer
{
public:
    Lock QueueLock;

    Renderer();
    ~Renderer();

    void NewFrame();
    void SwapBuffers();
    void ResetRenderer();

    void DrawMeshQueue();
    void DrawInterfaceQueue();

    // These functions can be called from other threads when adding to the
    // rendering queue that runs on the main thread. For this reason,
    // Each of these functions locks and unlocks their respective map/set
    // before modifying them at all. ProcessMeshQueues() reads from these
    // map/set queues and then clears them, requiring a plain mutex for R/W.
    void AddMeshToQueue(glm::ivec3 index, const Mesh &mesh);
    void UpdateMeshInQueue(glm::ivec3 index);
    void DeleteMeshFromQueue(glm::ivec3 index);
    void ProcessMeshQueues();

private:
    WindowHandler *m_WindowHandler;
    InterfaceHandler *m_InterfaceHandler;
    Camera *m_Camera;

    bool m_Resetting = false;
    unsigned int m_DrawCallsPerFrame = 0;

    glm::vec3 m_OverlayColor = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_BackgroundColor = {0.0f, 0.0f, 0.0f};

    std::unordered_map<glm::ivec3, Mesh> m_MeshMap;

    // Queues for safe mesh management
    std::unordered_map<glm::ivec3, Mesh> m_MeshesToAdd;
    std::unordered_set<glm::ivec3> m_MeshesToUpdate;
    std::unordered_set<glm::ivec3> m_MeshesToDelete;

public:
    glm::vec3 OverlayColor() { return m_OverlayColor; }
    void SetOverlayColor(const glm::vec3 &OverlayColor) { m_OverlayColor = OverlayColor; }

    glm::vec3 BackgroundColor() { return m_BackgroundColor; }
    void SetBackgroundColor(const glm::vec3 &BackgroundColor) { m_BackgroundColor = BackgroundColor; }

    bool IsResetting() { return m_Resetting; }
    void SetResetting(bool Resetting) { m_Resetting = Resetting; }

    unsigned int DrawCallsPerFrame() { return m_DrawCallsPerFrame; }
    void SetDrawCallsPerFrame(unsigned int DrawCallsPerFrame) { m_DrawCallsPerFrame = DrawCallsPerFrame; }

    void SetWindowHandler(WindowHandler *WindowHandler) { m_WindowHandler = WindowHandler; }
    void SetInterfaceHandler(InterfaceHandler *InterfaceHandler) { m_InterfaceHandler = InterfaceHandler; }
    void SetCamera(Camera *Camera) { m_Camera = Camera; }
};
