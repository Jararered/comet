#include "TextureMap.h"

void TextureMap::Initialize() { Get(); }

void TextureMap::Configure(int width, int height, int resolution)
{
    TextureMap &map = Get();

    if (!(width > 0) || !(width < 10000) || !(height > 0) || !(height < 10000))
    {
        std::cout << "TextureMap::Configure(): Unexpected image parameters.\n";
        return;
    }

    map.m_Width = width;
    map.m_Height = height;
    map.m_Resolution = resolution;

    float x, y, left, bottom, right, top;

    for (int j = 0; j < map.m_Height; j += resolution)
    {
        for (int i = 0; i < map.m_Width; i += resolution)
        {
            x = static_cast<float>(i);
            y = static_cast<float>(j);

            left = x / static_cast<float>(map.m_Width);
            bottom = y / static_cast<float>(map.m_Height);

            right = (x + static_cast<float>(resolution)) / static_cast<float>(map.m_Width);
            top = (y + static_cast<float>(resolution)) / static_cast<float>(map.m_Height);

            map.m_Coords.push_back({left, right, bottom, top});
        }
    }
}

std::vector<std::vector<float>> TextureMap::TextureCoordinates() { return Get().m_Coords; }

glm::vec2 TextureMap::TopLeft(int index) { return {Get().m_Coords[index][0], Get().m_Coords[index][2]}; }

glm::vec2 TextureMap::TopRight(int index) { return {Get().m_Coords[index][1], Get().m_Coords[index][2]}; }

glm::vec2 TextureMap::BottomLeft(int index) { return {Get().m_Coords[index][0], Get().m_Coords[index][3]}; }

glm::vec2 TextureMap::BottomRight(int index) { return {Get().m_Coords[index][1], Get().m_Coords[index][3]}; }
