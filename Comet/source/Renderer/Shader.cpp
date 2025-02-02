#include "Shader.h"

#include <fstream>
#include <iostream>
#include <string>

void Shader::Bind()
{
    glUseProgram(m_ID);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::Create(const std::string& vertFile, const std::string& fragFile)
{
    m_ID = glCreateProgram();

    // Write to console which files are being compiled
    std::cout << "Shader::Create(): Compiling " << vertFile << " and " << fragFile << "\n";

    unsigned int vertexID;
    unsigned int fragmentID;
    int successvert;
    int successfrag;
    int successlink;
    char glShaderInfoLog[512];

    // Vertex Shader
    {
        std::ifstream file(vertFile);
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        const char* contentsChar = contents.c_str();
        vertexID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexID, 1, &contentsChar, NULL);
        glCompileShader(vertexID);
    }

    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &successvert);
    if (!successvert)
    {
        glGetShaderInfoLog(vertexID, 512, NULL, glShaderInfoLog);
        std::cout << "Shader::Create(): Error compiling vertex shader:\n" << glShaderInfoLog << "\n";
    }

    // Fragment Shader
    {
        std::ifstream file(fragFile);
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        const char* contentsChar = contents.c_str();
        fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentID, 1, &contentsChar, NULL);
        glCompileShader(fragmentID);
    }

    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &successfrag);
    if (!successfrag)
    {
        glGetShaderInfoLog(fragmentID, 512, NULL, glShaderInfoLog);
        std::cout << "Shader::Create(): Error compiling fragment shader:\n" << glShaderInfoLog << "\n";
    }

    glAttachShader(m_ID, vertexID);
    glAttachShader(m_ID, fragmentID);
    glLinkProgram(m_ID);

    glGetProgramiv(m_ID, GL_LINK_STATUS, &successlink);
    if (!successlink)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, glShaderInfoLog);
        std::cout << "Shader::Create(): Error linking vertex and fragment shaders:\n" << glShaderInfoLog << "\n";
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    if (successvert && successfrag && successlink)
    {
        std::cout << "Shader::Create(): Successfully compiled shader: " << m_ID << "\n";
    }
}

void Shader::Delete()
{
    glDeleteProgram(m_ID);
}

unsigned int Shader::GetID()
{
    return m_ID;
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformMap.find(name) != m_UniformMap.end())
    {
        return m_UniformMap[name];
    }

    int location = glGetUniformLocation(m_ID, name.c_str());

    if (location == -1)
    {
        std::cout << "Shader::GetUniformLocation(): Uniform " << name << " not found. Ignoring...\n";
    }

    m_UniformMap[name] = location;

    return location;
}
