#include "Renderer.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <raymath.h>
#include <rlgl.h>

RenderLock Renderer::QueueLock;

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
    ::Camera3D raylibCam = Comet::ViewCamera::GetRaylibCamera();
    BeginMode3D(raylibCam);
    rlDisableBackfaceCulling();

    if (Get().m_BlockMaterial.shader.id > 0)
    {
        auto drawChunkMesh = [&drawCallCount](const glm::ivec3& index, GameMesh& mesh)
        {
            mesh.Update();

            ::Shader rShader = Get().m_BlockMaterial.shader;
            int locOverlay = GetShaderLocation(rShader, "u_OverlayColor");
            if (locOverlay >= 0)
            {
                SetShaderValue(rShader, locOverlay, &Get().m_OverlayColor, SHADER_UNIFORM_VEC3);
            }

            Matrix transform = MatrixTranslate(
                static_cast<float>(index.x) * 16.0f,
                0.0f,
                static_cast<float>(index.z) * 16.0f
            );

            DrawMesh(mesh.GetRaylibMesh(), Get().m_BlockMaterial, transform);
            drawCallCount++;
        };

        for (auto& [index, mesh] : Get().m_MeshMap)
        {
            drawChunkMesh(index, mesh);
        }

        rlDrawRenderBatchActive();
        rlDisableDepthMask();
        BeginBlendMode(BLEND_ALPHA);
        for (auto& [index, mesh] : Get().m_WaterMeshMap)
        {
            drawChunkMesh(index, mesh);
        }
        EndBlendMode();
        rlEnableDepthMask();
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
    QueueLock.AddQueue.lock();
    Get().m_MeshesToAdd.insert_or_assign(index, mesh);
    QueueLock.AddQueue.unlock();
}

void Renderer::AddWaterMeshToQueue(glm::ivec3 index, const GameMesh& mesh)
{
    QueueLock.AddQueue.lock();
    Get().m_WaterMeshesToAdd.insert_or_assign(index, mesh);
    QueueLock.AddQueue.unlock();
}

void Renderer::UpdateMeshInQueue(glm::ivec3 index)
{
    QueueLock.UpdateQueue.lock();
    Get().m_MeshesToUpdate.insert(index);
    QueueLock.UpdateQueue.unlock();
}

void Renderer::DeleteMeshFromQueue(glm::ivec3 index)
{
    QueueLock.DeleteQueue.lock();
    Get().m_MeshesToDelete.insert(index);
    QueueLock.DeleteQueue.unlock();
}

void Renderer::ProcessMeshQueues()
{
    QueueLock.AddQueue.lock();
    for (const auto& [index, mesh] : Get().m_MeshesToAdd)
    {
        Get().m_MeshMap.insert_or_assign(index, mesh);
        Get().m_MeshMap.at(index).Initialize();
    }
    Get().m_MeshesToAdd.clear();
    for (const auto& [index, mesh] : Get().m_WaterMeshesToAdd)
    {
        if (mesh.GetIndices().empty())
        {
            if (auto entry = Get().m_WaterMeshMap.find(index); entry != Get().m_WaterMeshMap.end())
            {
                entry->second.Finalize();
                Get().m_WaterMeshMap.erase(entry);
            }
            continue;
        }

        Get().m_WaterMeshMap.insert_or_assign(index, mesh);
        Get().m_WaterMeshMap.at(index).Initialize();
    }
    Get().m_WaterMeshesToAdd.clear();
    QueueLock.AddQueue.unlock();

    QueueLock.UpdateQueue.lock();
    for (const auto& index : Get().m_MeshesToUpdate)
    {
        if (auto entry = Get().m_MeshMap.find(index); entry != Get().m_MeshMap.end())
        {
            entry->second.UpdateGeometry();
        }
        if (auto entry = Get().m_WaterMeshMap.find(index); entry != Get().m_WaterMeshMap.end())
        {
            entry->second.UpdateGeometry();
        }
    }
    Get().m_MeshesToUpdate.clear();
    QueueLock.UpdateQueue.unlock();

    QueueLock.DeleteQueue.lock();
    for (const auto& index : Get().m_MeshesToDelete)
    {
        if (auto entry = Get().m_MeshMap.find(index); entry != Get().m_MeshMap.end())
        {
            entry->second.Finalize();
            Get().m_MeshMap.erase(index);
        }
        if (auto entry = Get().m_WaterMeshMap.find(index); entry != Get().m_WaterMeshMap.end())
        {
            entry->second.Finalize();
            Get().m_WaterMeshMap.erase(index);
        }
    }
    Get().m_MeshesToDelete.clear();
    QueueLock.DeleteQueue.unlock();
}

void Renderer::Update()
{
    NewFrame();
    DrawMeshQueue();
    rlDrawRenderBatchActive();
    DrawInterfaceQueue();
    EndDrawing();
}
