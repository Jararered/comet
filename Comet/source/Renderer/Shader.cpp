#include "Shader.h"

#include <iostream>

void GameShader::Bind()
{
    BeginShaderMode(m_Shader);
}

void GameShader::Unbind()
{
    EndShaderMode();
}

void GameShader::Create(const std::string& vertFile, const std::string& fragFile)
{
    std::cout << "GameShader::Create(): Compiling " << vertFile << " and " << fragFile << "\n";

    m_Shader = LoadShader(vertFile.c_str(), fragFile.c_str());

    if (!IsShaderValid(m_Shader))
    {
        std::cout << "GameShader::Create(): Failed to load shader.\n";
    }
    else
    {
        std::cout << "GameShader::Create(): Successfully compiled shader.\n";
    }

    m_Shader.locs[SHADER_LOC_VERTEX_POSITION] = GetShaderLocationAttrib(m_Shader, "vertexPosition");
    m_Shader.locs[SHADER_LOC_VERTEX_TEXCOORD01] = GetShaderLocationAttrib(m_Shader, "vertexTexCoord");
    m_Shader.locs[SHADER_LOC_VERTEX_NORMAL] = GetShaderLocationAttrib(m_Shader, "vertexNormal");
    m_Shader.locs[SHADER_LOC_VERTEX_COLOR] = GetShaderLocationAttrib(m_Shader, "vertexColor");
    m_Shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(m_Shader, "mvp");
    m_Shader.locs[SHADER_LOC_MAP_DIFFUSE] = GetShaderLocation(m_Shader, "texture0");

    std::cout << "GameShader::Create(): locations"
              << " position=" << m_Shader.locs[SHADER_LOC_VERTEX_POSITION]
              << " texcoord=" << m_Shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]
              << " normal=" << m_Shader.locs[SHADER_LOC_VERTEX_NORMAL]
              << " color=" << m_Shader.locs[SHADER_LOC_VERTEX_COLOR]
              << " mvp=" << m_Shader.locs[SHADER_LOC_MATRIX_MVP]
              << " texture=" << m_Shader.locs[SHADER_LOC_MAP_DIFFUSE] << "\n";
}

void GameShader::Delete()
{
    UnloadShader(m_Shader);
}

int GameShader::GetUniformLocation(const std::string& name)
{
    if (m_UniformMap.find(name) != m_UniformMap.end())
    {
        return m_UniformMap[name];
    }

    int location = GetShaderLocation(m_Shader, name.c_str());

    if (location == -1)
    {
        std::cout << "GameShader::GetUniformLocation(): Uniform " << name << " not found. Ignoring...\n";
    }

    m_UniformMap[name] = location;
    return location;
}
