#include "RenderInterface.h"

#include "input/Input.h"

#include "imgui.h"
#include "imgui_internal.h"

using namespace Comet;

RenderInterface::RenderInterface()
{
    InterfaceHandler::AddInterface(this);

    m_RenderDistance = World::RenderDistance();
    m_RenderShape = World::RenderShape();
    m_WorldSeed = ChunkGenerator::Seed();
    m_OverlayColor = Renderer::OverlayColor();
    m_BackgroundColor = Renderer::BackgroundColor();
}

RenderInterface::~RenderInterface() {}

void RenderInterface::Draw()
{
    ImGui::Begin("Render Menu");
    if (ImGui::Button("Play"))
    {
        Input::CaptureCursor();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Renderer"))
    {
        Renderer::ResetRenderer();
        World::Finalize();
        Renderer::SetResetting(false);

        World::Initialize();
        World::InitializeThread();
    }
    ImGui::SameLine();
    if (ImGui::Button("Exit"))
    {
        Engine::SetShouldClose(true);
    }

    ImGui::Text("Render Modes: ");
    ImGui::SameLine();
    if (ImGui::Button("Wireframe"))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    ImGui::SameLine();

    if (ImGui::Button("Filled"))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (ImGui::SliderInt("Render Distance", &m_RenderDistance, 0, 32))
    {
        World::SetRenderDistance(m_RenderDistance);
    }

    ImGui::Text("Render Shape: ");
    ImGui::SameLine();
    if (ImGui::Button("Square"))
    {
        World::SetRenderShape(RenderShape::Square);
    }
    ImGui::SameLine();
    if (ImGui::Button("Circle"))
    {
        World::SetRenderShape(RenderShape::Circle);
    }

    if (ImGui::InputInt("World Seed", &m_WorldSeed))
    {
        ChunkGenerator::SetSeed(m_WorldSeed);
    }

    if (ImGui::SliderFloat3("Overlay Color", &m_OverlayColor[0], -1.0f, 1.0f))
    {
        Renderer::SetOverlayColor(m_OverlayColor);
    }
    if (ImGui::SliderFloat3("Background Color", &m_BackgroundColor[0], 0.0f, 1.0f))
    {
        Renderer::SetBackgroundColor(m_BackgroundColor);
    }

    ImGui::Text("Physics Time: %.8f", Utilities::Statistics::PhysicsTime);
    ImGui::Text("Render Time: %.8f", Utilities::Statistics::RenderTime);

    ImGui::Text("Performance: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Text("Draws/Frame: %d", Renderer::DrawCallsPerFrame());

    ImGui::End();
}
