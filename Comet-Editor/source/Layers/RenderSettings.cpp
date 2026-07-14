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

    ImGui::Text("Performance: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Text("Draws/Frame: %d", Renderer::DrawCallsPerFrame());
    if (m_World != nullptr && m_World->GetMainPlayer() != nullptr)
    {
        ImGui::Text("Fly Speed: %.2fx", m_World->GetMainPlayer()->FlySpeedMultiplier());
    }
    const glm::vec3 cameraPosition = Comet::ViewCamera::Position();
    const glm::vec3 cameraForward = Comet::ViewCamera::ForwardVector();
    ImGui::Text("Camera: %.1f %.1f %.1f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    ImGui::Text("Forward: %.2f %.2f %.2f", cameraForward.x, cameraForward.y, cameraForward.z);

    ImGui::End();
}
