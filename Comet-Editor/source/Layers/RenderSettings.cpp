#include "RenderSettings.h"

#include "Entities/Player.h"

#include <Input/Input.h>

#include <cstdio>
#include <raygui.h>
#include <raylib.h>

void Settings::Draw()
{
    constexpr float panelX = 10.0f;
    constexpr float panelY = 10.0f;
    constexpr float panelWidth = 280.0f;
    constexpr float panelHeight = 220.0f;
    constexpr float pad = 12.0f;
    constexpr float rowHeight = 24.0f;
    constexpr float rowGap = 6.0f;

    GuiPanel(Rectangle{panelX, panelY, panelWidth, panelHeight}, "Render Menu");

    float y = panelY + 36.0f;
    const float contentX = panelX + pad;
    const float contentWidth = panelWidth - pad * 2.0f;

    if (GuiButton(Rectangle{contentX, y, contentWidth, rowHeight}, "Play"))
    {
        Input::CaptureCursor();
    }
    y += rowHeight + rowGap;

    bool wireMeshEnabled = m_Renderer != nullptr && m_Renderer->WireMeshEnabled();
    if (GuiCheckBox(Rectangle{contentX, y, rowHeight, rowHeight}, "Wire Mesh", &wireMeshEnabled) && m_Renderer != nullptr)
    {
        m_Renderer->SetWireMeshEnabled(wireMeshEnabled);
    }
    y += rowHeight + rowGap;

    char line[128];
    std::snprintf(line, sizeof(line), "Performance: %d FPS", GetFPS());
    GuiLabel(Rectangle{contentX, y, contentWidth, rowHeight}, line);
    y += rowHeight + rowGap;

    std::snprintf(line, sizeof(line), "Draws/Frame: %d", m_Renderer != nullptr ? m_Renderer->DrawCallsPerFrame() : 0);
    GuiLabel(Rectangle{contentX, y, contentWidth, rowHeight}, line);
    y += rowHeight + rowGap;

    if (m_World != nullptr && m_World->GetMainPlayer() != nullptr)
    {
        std::snprintf(line, sizeof(line), "Fly Speed: %.2fx", m_World->GetMainPlayer()->FlySpeedMultiplier());
        GuiLabel(Rectangle{contentX, y, contentWidth, rowHeight}, line);
        y += rowHeight + rowGap;
    }

    const glm::vec3 cameraPosition = m_Camera != nullptr ? m_Camera->Position() : glm::vec3{0.0f};
    const glm::vec3 cameraForward = m_Camera != nullptr ? m_Camera->ForwardVector() : glm::vec3{0.0f};
    std::snprintf(line, sizeof(line), "Camera: %.1f %.1f %.1f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    GuiLabel(Rectangle{contentX, y, contentWidth, rowHeight}, line);
    y += rowHeight + rowGap;

    std::snprintf(line, sizeof(line), "Forward: %.2f %.2f %.2f", cameraForward.x, cameraForward.y, cameraForward.z);
    GuiLabel(Rectangle{contentX, y, contentWidth, rowHeight}, line);
}