#pragma once

#include <glad/gl.h>

class Texture
{
public:
    void Create(const char* filepath);
    void Delete();
    void Bind();
    void Unbind();

private:
    int m_Width;
    int m_Height;
    int m_ChannelCount;
    unsigned int m_ID;

public:
    int Width() const { return m_Width; }
    int Height() const { return m_Height; }
    int ChannelCount() const { return m_ChannelCount; }
    unsigned int ID() const { return m_ID; }
};
