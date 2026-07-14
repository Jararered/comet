#include "Texture.h"

#include <iostream>

void GameTexture::Bind()
{
}

void GameTexture::Unbind()
{
}

void GameTexture::Create(const std::string& filepath)
{
    Image image = LoadImage(filepath.c_str());
    if (image.data == nullptr)
    {
        std::cout << "GameTexture::Create(): Could not load image file from " << filepath << "\n";
        return;
    }

    m_Texture = LoadTextureFromImage(image);
    UnloadImage(image);

    SetTextureFilter(m_Texture, TEXTURE_FILTER_POINT);
    SetTextureWrap(m_Texture, TEXTURE_WRAP_REPEAT);

    m_Width = m_Texture.width;
    m_Height = m_Texture.height;
}

void GameTexture::Delete()
{
    UnloadTexture(m_Texture);
    m_Width = 0;
    m_Height = 0;
}
