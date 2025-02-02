#pragma once

#include <Layer/Layer.h>
#include <Layer/LayerManager.h>
#include <Renderer/Mesh.h>
#include <Renderer/Shader.h>
#include <Renderer/Texture.h>
#include <Renderer/Vertex.h>
#include <Window.h>

#include <vector>

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
