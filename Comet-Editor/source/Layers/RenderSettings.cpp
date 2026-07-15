#include "RenderSettings.h"

#include "Entities/Player.h"

#include <Input/Input.h>
#include <Utilities.h>

void Settings::Draw()
{
    ImGui::Begin("Render Menu");
    if (ImGui::Button("Play"))
    {
        Input::CaptureCursor();
    }

    bool wireMeshEnabled = m_Renderer != nullptr && m_Renderer->WireMeshEnabled();
    if (ImGui::Checkbox("Wire Mesh", &wireMeshEnabled) && m_Renderer != nullptr)
    {
        m_Renderer->SetWireMeshEnabled(wireMeshEnabled);
    }

    ImGui::Text("Performance: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Text("Draws/Frame: %d", m_Renderer != nullptr ? m_Renderer->DrawCallsPerFrame() : 0);
    if (m_World != nullptr && m_World->GetMainPlayer() != nullptr)
    {
        ImGui::Text("Fly Speed: %.2fx", m_World->GetMainPlayer()->FlySpeedMultiplier());
    }
    const glm::vec3 cameraPosition = m_Camera != nullptr ? m_Camera->Position() : glm::vec3{0.0f};
    const glm::vec3 cameraForward = m_Camera != nullptr ? m_Camera->ForwardVector() : glm::vec3{0.0f};
    ImGui::Text("Camera: %.1f %.1f %.1f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    ImGui::Text("Forward: %.2f %.2f %.2f", cameraForward.x, cameraForward.y, cameraForward.z);

    ImGui::End();
}
