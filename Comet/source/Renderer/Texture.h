#pragma once

#include <raylib.h>

#include <string>

class GameTexture
{
public:
    void Create(const std::string& filepath);
    void Delete();
    void Bind();
    void Unbind();

private:
    ::Texture2D m_Texture = {0};
    int m_Width = 0;
    int m_Height = 0;

public:
    int Width() const { return m_Width; }
    int Height() const { return m_Height; }
    ::Texture2D GetTexture() const { return m_Texture; }
};
