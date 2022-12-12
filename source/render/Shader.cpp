#include "Shader.h"

Shader::Shader() {}

Shader::Shader(const char *vertFile, const char *fragFile) { Create(vertFile, fragFile); }

Shader::~Shader() {}

void Shader::Bind() { glUseProgram(m_ID); }

void Shader::Unbind() { glUseProgram(0); }

void Shader::Create(const char *vertFile, const char *fragFile)
{
    m_ID = glCreateProgram();

    unsigned int vertexID;
    unsigned int fragmentID;
    int successvert;
    int successfrag;
    int successlink;
    char infoLog[512];

    // Vertex Shader
    {
        std::ifstream file(vertFile);
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        const char *contentsChar = contents.c_str();
        vertexID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexID, 1, &contentsChar, NULL);
        glCompileShader(vertexID);
    }

    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &successvert);
    if (!successvert)
    {
        glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
    }

    // Fragment Shader
    {
        std::ifstream file(fragFile);
        std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        const char *contentsChar = contents.c_str();
        fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentID, 1, &contentsChar, NULL);
        glCompileShader(fragmentID);
    }

    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &successfrag);
    if (!successfrag)
    {
        glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n";
    }

    glAttachShader(m_ID, vertexID);
    glAttachShader(m_ID, fragmentID);
    glLinkProgram(m_ID);


    glGetProgramiv(m_ID, GL_LINK_STATUS, &successlink);
    if (!successlink)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    if (successvert && successfrag && successlink)
    {
        std::cout << "Successfully compiled shader: " << m_ID << "\n";
    }
}

void Shader::Delete() { glDeleteProgram(m_ID); }

unsigned int Shader::GetID() { return m_ID; }

int Shader::GetUniformLocation(const std::string &name)
{
    if (m_UniformMap.find(name) != m_UniformMap.end())
    {
        return m_UniformMap[name];
    }

    int location = glGetUniformLocation(m_ID, name.c_str());

    if (location == -1)
    {
        std::cout << "Uniform " << name << " not found. Ignoring...\n";
    }

    m_UniformMap[name] = location;

    return location;
}
