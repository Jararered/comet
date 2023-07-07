#include "RenderSettings.h"

#include <Input/Input.h>
#include <Utilities.h>

void Settings::Draw()
{
    ImGui::Begin("Render Menu");
    if (ImGui::Button("Play"))
    {
        Input::CaptureCursor();
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

    ImGui::Text("Performance: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Text("Draws/Frame: %d", Renderer::DrawCallsPerFrame());

    ImGui::End();
}
