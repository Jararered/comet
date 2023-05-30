#pragma once

#include <glad/gl.h>

#include <string>
#include <unordered_map>

class Shader
{
public:
    Shader();
    Shader(const char* vertFile, const char* fragFile);
    ~Shader();

    void Bind();
    void Unbind();

    void Create(const char* vertFile, const char* fragFile);
    void Delete();

    unsigned int GetID();

    int GetUniformLocation(const std::string& name);

private:
    unsigned int m_ID;
    std::unordered_map<std::string, int> m_UniformMap;
};
