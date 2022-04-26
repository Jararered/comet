#include "Renderer.h"

Lock Renderer::QueueLock;

void Renderer::Initialize()
{
    Instance();

    // Enables z buffer depth testing, prevents incorrect depth rendering
    glEnable(GL_DEPTH_TEST);

    // Culls the back face of geometry
    glCullFace(GL_BACK);

    // For transparent objects, enables alpha values are considered when drawing.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(WindowHandler::Window(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Instance().m_BackgroundColor = glm::vec3(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f);
}

void Renderer::NewFrame()
{
    // Clearing the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Background color
    glClearColor(Instance().m_BackgroundColor.x, Instance().m_BackgroundColor.y, Instance().m_BackgroundColor.z, 0.0f);
}

void Renderer::SwapBuffers() { glfwSwapBuffers(glfwGetCurrentContext()); }

void Renderer::ResetRenderer()
{
    SetResetting(true);

    // Resetting all render queues
    Instance().m_MeshMap.clear();
    Instance().m_MeshesToAdd.clear();
    Instance().m_MeshesToDelete.clear();
    Instance().m_MeshesToUpdate.clear();
}

void Renderer::DrawMeshQueue()
{
    unsigned int drawCalls = 0;

    unsigned int shaderID;
    auto &renderer = Instance();

    ProcessMeshQueues();

    // Binding the texture map
    glBindTexture(GL_TEXTURE_2D, 1);

    // Solid Geometry Loop
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    for (auto &[index, mesh] : Instance().m_MeshMap)
    {
        shaderID = mesh.Shader()->GetID();

        mesh.Update();

        // Binding the next mesh in queue
        mesh.Bind();

        // Uniforms
        glUniform3iv(glGetUniformLocation(shaderID, "u_Index"), 1, &index[0]);
        glUniform3fv(glGetUniformLocation(shaderID, "u_OverlayColor"), 1, &OverlayColor()[0]);
        glUniform1f(glGetUniformLocation(shaderID, "u_Transparency"), mesh.Transparency());
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ModelMatrix"), 1, GL_FALSE, &mesh.ModelMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ViewMatrix"), 1, GL_FALSE, &Camera::ViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ProjMatrix"), 1, GL_FALSE, &Camera::ProjMatrix()[0][0]);
        glUniform1i(glGetUniformLocation(shaderID, "u_Texture"), 0);

        // Drawing mesh
        glDrawElements(GL_TRIANGLES, mesh.Count(), GL_UNSIGNED_INT, (void *)0);
        drawCalls++;
    }

    SetDrawCallsPerFrame(drawCalls);
}

void Renderer::DrawInterfaceQueue()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    InterfaceHandler::DrawInterfaces();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void Renderer::AddMeshToQueue(glm::ivec3 index, const Mesh &mesh)
{
    QueueLock.AddQueue.lock();
    Instance().m_MeshesToAdd.insert_or_assign(index, mesh);
    QueueLock.AddQueue.unlock();
}

void Renderer::UpdateMeshInQueue(glm::ivec3 index)
{
    QueueLock.UpdateQueue.lock();
    Instance().m_MeshesToUpdate.insert(index);
    QueueLock.UpdateQueue.unlock();
}

void Renderer::DeleteMeshFromQueue(glm::ivec3 index)
{
    QueueLock.DeleteQueue.lock();
    Instance().m_MeshesToDelete.insert(index);
    QueueLock.DeleteQueue.unlock();
}

void Renderer::ProcessMeshQueues()
{
    // Adding meshes to the queue
    QueueLock.AddQueue.lock();
    for (const auto &[index, mesh] : Instance().m_MeshesToAdd)
    {

        Instance().m_MeshMap.insert_or_assign(index, mesh);
        Instance().m_MeshMap.at(index).Initialize();
    }
    Instance().m_MeshesToAdd.clear();
    QueueLock.AddQueue.unlock();

    // Updating meshes in the queue
    QueueLock.UpdateQueue.lock();
    for (const auto &index : Instance().m_MeshesToUpdate)
    {
        Instance().m_MeshMap.at(index).UpdateGeometry();
    }
    Instance().m_MeshesToUpdate.clear();
    QueueLock.UpdateQueue.unlock();

    // Deleting meshes in the queue
    QueueLock.DeleteQueue.lock();
    for (const auto &index : Instance().m_MeshesToDelete)
    {

        if (auto entry = Instance().m_MeshMap.find(index); entry != Instance().m_MeshMap.end())
        {
            // Instance().m_SolidMeshMap.at(index).Finalize();
            entry->second.Finalize();
            Instance().m_MeshMap.erase(index);
        }
    }
    Instance().m_MeshesToDelete.clear();
    QueueLock.DeleteQueue.unlock();
}
