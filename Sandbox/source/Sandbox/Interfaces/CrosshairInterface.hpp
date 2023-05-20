#pragma once

#include <Comet/Interfaces/Interface.hpp>
#include <Comet/Interfaces/InterfaceManager.hpp>
#include <Comet/Renderer/Mesh.hpp>
#include <Comet/Renderer/Shader.hpp>
#include <Comet/Renderer/Texture.hpp>
#include <Comet/Renderer/Vertex.hpp>
#include <Comet/Window.hpp>

class CrosshairInterface : public Interface
{
public:
    CrosshairInterface();
    ~CrosshairInterface();

    void Draw();

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    Shader m_Shader;
    Texture m_Texture;
    Mesh m_Mesh;
};
