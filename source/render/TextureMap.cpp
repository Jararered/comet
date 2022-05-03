#include "TextureMap.h"

TextureMap::TextureMap() {}

TextureMap::~TextureMap() {}

void TextureMap::Configure(int width, int height, int resolution)
{
    if (!(width > 0) || !(width < 10000) || !(height > 0) || !(height < 10000))
    {
        std::cout << "TextureMap::Configure(): Unexpected image parameters.\n";
        return;
    }

    m_Width = width;
    m_Height = height;
    m_Resolution = resolution;

    float x, y, left, bottom, right, top;

    for (int j = 0; j < m_Height; j += resolution)
    {
        for (int i = 0; i < m_Width; i += resolution)
        {
            x = static_cast<float>(i);
            y = static_cast<float>(j);

            left = x / static_cast<float>(m_Width);
            bottom = y / static_cast<float>(m_Height);

            right = (x + static_cast<float>(resolution)) / static_cast<float>(m_Width);
            top = (y + static_cast<float>(resolution)) / static_cast<float>(m_Height);

            m_Coords.push_back({left, right, bottom, top});
        }
    }
}

std::vector<std::vector<float>> TextureMap::TextureCoordinates() { return m_Coords; }

glm::vec2 TextureMap::TopLeft(int index) { return {m_Coords[index][0], m_Coords[index][2]}; }

glm::vec2 TextureMap::TopRight(int index) { return {m_Coords[index][1], m_Coords[index][2]}; }

glm::vec2 TextureMap::BottomLeft(int index) { return {m_Coords[index][0], m_Coords[index][3]}; }

glm::vec2 TextureMap::BottomRight(int index) { return {m_Coords[index][1], m_Coords[index][3]}; }
