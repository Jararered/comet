#pragma once

#include <raylib.h>

#include <string>
#include <unordered_map>

class GameShader
{
public:
    void Create(const std::string& vertFile, const std::string& fragFile);
    void Delete();
    void Bind();
    void Unbind();

    ::Shader GetID() const { return m_Shader; }
    int GetUniformLocation(const std::string& name);

private:
    ::Shader m_Shader = {0};
    std::unordered_map<std::string, int> m_UniformMap;
};
