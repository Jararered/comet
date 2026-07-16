#include "Crosshair.h"

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
    const float x = (static_cast<float>(GetScreenWidth()) - static_cast<float>(m_Texture.width)) * 0.5f;
    const float y = (static_cast<float>(GetScreenHeight()) - static_cast<float>(m_Texture.height)) * 0.5f;
    DrawTexture(m_Texture, static_cast<int>(x), static_cast<int>(y), WHITE);
}
