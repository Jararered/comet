#include "Mesh.h"

#include <glfw/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, Shader* shader)
    : p_Vertices(vertices), p_Indices(indices), p_Shader(shader), m_OnGPU(false), m_ModelMatrix(1.0f)
{
}

void Mesh::Bind()
{
    glUseProgram(p_Shader->GetID());
    glBindVertexArray(m_VAO);
}

void Mesh::Unbind()
{
    glUseProgram(0);
    glBindVertexArray(0);
}

void Mesh::Initialize()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * p_Vertices->size() * sizeof(Vertex), (void*)0, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, p_Vertices->size() * sizeof(Vertex), p_Vertices->data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * p_Indices->size() * sizeof(unsigned int), (void*)0, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, p_Indices->size() * sizeof(unsigned int), p_Indices->data());

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TextureCoordinate)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_OnGPU = true;
}

void Mesh::Update()
{
}

void Mesh::UpdateGeometry()
{
    // Binding
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    // Buffer geometry data
    glBufferSubData(GL_ARRAY_BUFFER, 0, p_Vertices->size() * sizeof(Vertex), p_Vertices->data());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, p_Indices->size() * sizeof(unsigned int), p_Indices->data());

    // Unbinding everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Finalize()
{
    // Deleting all buffers
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);

    m_OnGPU = false;
}
