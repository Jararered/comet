#pragma once

#include <Layer/Layer.hpp>
#include <Layer/LayerManager.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Vertex.hpp>
#include <Window.hpp>

class Crosshair : public Layer
{
public:
    Crosshair();
    ~Crosshair() override = default;

    void Draw() override;

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    Shader m_Shader;
    Texture m_Texture;
    Mesh m_Mesh;
};
