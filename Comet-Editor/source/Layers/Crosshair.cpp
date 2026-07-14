#include "Crosshair.h"

#include <imgui.h>

Crosshair::Crosshair()
{
    const std::filesystem::path res = EditorResourcesRoot();
    std::string texturePath = (res / "Textures" / "crosshair.png").string();
    Image image = LoadImage(texturePath.c_str());
    m_Texture = LoadTextureFromImage(image);
    UnloadImage(image);
}

void Crosshair::Draw()
{
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 pos = ImVec2(
        (screenSize.x - static_cast<float>(m_Texture.width)) * 0.5f,
        (screenSize.y - static_cast<float>(m_Texture.height)) * 0.5f
    );
    ImGui::GetForegroundDrawList()->AddImage(
        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(m_Texture.id)),
        pos,
        ImVec2(pos.x + static_cast<float>(m_Texture.width), pos.y + static_cast<float>(m_Texture.height))
    );
}