#pragma once

#include <glad/gl.h>

#include <string>
#include <unordered_map>

class Shader
{
public:
    void Create(const char* vertFile, const char* fragFile);
    void Delete();
    void Bind();
    void Unbind();

    unsigned int GetID();
    int GetUniformLocation(const std::string& name);

private:
    unsigned int m_ID;
    std::unordered_map<std::string, int> m_UniformMap;
};
