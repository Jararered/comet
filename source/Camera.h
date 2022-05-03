#pragma once

#include <comet.pch>

#include "Engine.h"
#include "handlers/MouseHandler.h"
#include "handlers/WindowHandler.h"

class Camera
{
public:
    Camera();
    ~Camera();

    void Update();
    void CalcViewMatrix();
    void CalcProjMatrix();

    void Move();
    void Rotate();

    /* World Variables */
    glm::vec3 POSITIVE_X = {1.0f, 0.0f, 0.0f};
    glm::vec3 POSITIVE_Y = {0.0f, 1.0f, 0.0f};
    glm::vec3 POSITIVE_Z = {0.0f, 0.0f, 1.0f};
    glm::vec3 WORLD_ORIGIN = {0.0f, 0.0f, 0.0f};

private:
    float m_FOV = glm::radians(85.0f);
    float m_Aspect = 16.0f / 9.0f;
    float m_Near = 0.1f;
    float m_Far = 1000.0f;

    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};

    glm::vec3 m_ForwardVector = {0.0f, 0.0f, -1.0f};

    glm::mat4 m_ViewMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    glm::mat4 m_ProjMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

public:
    float FOV() { return m_FOV; }
    void SetFOV(float FOV) { m_FOV = FOV; }

    float Aspect() { return m_Aspect; }
    void SetAspect(float Aspect) { m_Aspect = Aspect; }

    float Near() { return m_Near; }
    void SetNear(float Near) { m_Near = Near; }

    float Far() { return m_Far; }
    void SetFar(float Far) { m_Far = Far; }

    glm::vec3 Position() { return m_Position; }
    void SetPosition(glm::vec3 Position) { m_Position = Position; }

    glm::mat4 ViewMatrix() { return m_ViewMatrix; }
    void SetViewMatrix(glm::mat4 ViewMatrix) { m_ViewMatrix = ViewMatrix; }

    glm::mat4 ProjMatrix() { return m_ProjMatrix; }
    void SetProjMatrix(glm::mat4 ProjMatrix) { m_ProjMatrix = ProjMatrix; }

    glm::vec3 ForwardVector() { return m_ForwardVector; }
    void SetForwardVector(const glm::vec3 &ForwardVector) { m_ForwardVector = ForwardVector; }
};
