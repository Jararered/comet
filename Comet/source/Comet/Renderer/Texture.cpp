#include "Texture.hpp"

#include <stb_image.h>

#include <iostream>

Texture::Texture() {}

Texture::Texture(const char* filepath) { Create(filepath); }

Texture::~Texture() {}

void Texture::Bind() { glBindTexture(GL_TEXTURE_2D, m_ID); }

void Texture::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::Create(const char* filepath)
{
    // Texture Setup
    // stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &m_ChannelCount, 0);
    if (!data)
    {
        std::cout << "Texture::Create(): Could not load image file from " << filepath << "\n";
        return;
    }

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Repeat options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // No upscaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::Delete()
{
    m_Width = 0;
    m_Height = 0;
    m_ChannelCount = 0;
    glDeleteTextures(1, &m_ID);
}
