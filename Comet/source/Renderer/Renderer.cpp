#include "Renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glfw/glfw3.h>

RenderLock Renderer::QueueLock;

void Renderer::Initialize()
{
    Get();

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Make textures repeat and not clamp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Get().m_BackgroundColor = glm::vec3(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f);
}

void Renderer::NewFrame()
{
    // Clearing the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Background color
    glClearColor(Get().m_BackgroundColor.x, Get().m_BackgroundColor.y, Get().m_BackgroundColor.z, 0.0f);
}

void Renderer::SwapBuffers()
{
    glfwSwapBuffers(glfwGetCurrentContext());
}

void Renderer::DrawMeshQueue()
{
    size_t drawCallCount = 0;

    uint16_t shaderID;
    auto& renderer = Get();

    ProcessMeshQueues();

    // Binding the texture map
    glBindTexture(GL_TEXTURE_2D, 1);

    // Solid Geometry Loop
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    for (auto& [index, mesh] : Get().m_MeshMap)
    {
        shaderID = mesh.GetShader()->GetID();

        mesh.Update();

        // Binding the next mesh in queue
        mesh.Bind();

        // Uniforms
        glUniform3iv(glGetUniformLocation(shaderID, "u_Index"), 1, &index[0]);
        glUniform3fv(glGetUniformLocation(shaderID, "u_OverlayColor"), 1, &OverlayColor()[0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ModelMatrix"), 1, GL_FALSE, &mesh.GetModelMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ViewMatrix"), 1, GL_FALSE, &Camera::ViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ProjMatrix"), 1, GL_FALSE, &Camera::ProjMatrix()[0][0]);
        glUniform1i(glGetUniformLocation(shaderID, "u_Texture"), 0);

        // Drawing mesh
        glDrawElements(GL_TRIANGLES, mesh.GetIndices()->size(), GL_UNSIGNED_INT, (void*)0);
        drawCallCount++;
    }

    SetDrawCallsPerFrame(drawCallCount);
}

void Renderer::DrawInterfaceQueue()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    LayerManager::Draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void Renderer::AddMeshToQueue(glm::ivec3 index, const Mesh& mesh)
{
    QueueLock.AddQueue.lock();
    Get().m_MeshesToAdd.insert_or_assign(index, mesh);
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
    // Adding meshes to the queue
    QueueLock.AddQueue.lock();
    for (const auto& [index, mesh] : Get().m_MeshesToAdd)
    {
        Get().m_MeshMap.insert_or_assign(index, mesh);
        Get().m_MeshMap.at(index).Initialize();
    }
    Get().m_MeshesToAdd.clear();
    QueueLock.AddQueue.unlock();

    // Updating meshes in the queue
    QueueLock.UpdateQueue.lock();
    for (const auto& index : Get().m_MeshesToUpdate)
    {
        Get().m_MeshMap.at(index).UpdateGeometry();
    }
    Get().m_MeshesToUpdate.clear();
    QueueLock.UpdateQueue.unlock();

    // Deleting meshes in the queue
    QueueLock.DeleteQueue.lock();
    for (const auto& index : Get().m_MeshesToDelete)
    {
        if (auto entry = Get().m_MeshMap.find(index); entry != Get().m_MeshMap.end())
        {
            entry->second.Finalize();
            Get().m_MeshMap.erase(index);
        }
    }
    Get().m_MeshesToDelete.clear();
    QueueLock.DeleteQueue.unlock();
}

void Renderer::Update()
{
    NewFrame();

    DrawMeshQueue();
    DrawInterfaceQueue();

    SwapBuffers();
}