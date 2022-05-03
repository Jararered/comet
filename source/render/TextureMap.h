#pragma once

#include <comet.pch>

class TextureMap
{
public:
    TextureMap();
    ~TextureMap();

    void Configure(int width, int height, int resolution);
    std::vector<std::vector<float>> TextureCoordinates();
    glm::vec2 TopLeft(int index);
    glm::vec2 TopRight(int index);
    glm::vec2 BottomLeft(int index);
    glm::vec2 BottomRight(int index);

private:
    unsigned int m_Width = 0;      // horizontal pixel count
    unsigned int m_Height = 0;     // vertical pixel count
    unsigned int m_Resolution = 0; // resolution of each texture

    // vector of 4x1 vectors (left, right, bottom, top coordinates)
    std::vector<std::vector<float>> m_Coords;
};
