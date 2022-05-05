#pragma once

#include <comet.pch>

class TextureMap
{
  public:
    static TextureMap &Get()
    {
        static TextureMap s_Instance;
        return s_Instance;
    }

    // Singleton approach, assuming that if game is using a texture map
    // then game is probably only going to be loading in a single texture.

    static void Initialize();
    static void Configure(int width, int height, int resolution);
    static std::vector<std::vector<float>> TextureCoordinates();
    static glm::vec2 TopLeft(int index);
    static glm::vec2 TopRight(int index);
    static glm::vec2 BottomLeft(int index);
    static glm::vec2 BottomRight(int index);

  private:
    TextureMap() {}
    TextureMap(TextureMap const &);
    void operator=(TextureMap const &) {}

    unsigned int m_Width = 0;      // horizontal pixel count
    unsigned int m_Height = 0;     // vertical pixel count
    unsigned int m_Resolution = 0; // resolution of each texture

    // vector of 4x1 vectors (left, right, bottom, top coordinates)
    std::vector<std::vector<float>> m_Coords;
};
