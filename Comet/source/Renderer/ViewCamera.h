#pragma once

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

#include <raylib.h>

namespace Comet
{

class ViewCamera
{
public:
    void Initialize();
    void Update();
    void CalcViewMatrix();
    void CalcProjMatrix();

    ::Camera3D GetRaylibCamera() const;

    /* World Variables */
    constexpr static glm::vec3 POSITIVE_X = {1.0f, 0.0f, 0.0f};
    constexpr static glm::vec3 POSITIVE_Y = {0.0f, 1.0f, 0.0f};
    constexpr static glm::vec3 POSITIVE_Z = {0.0f, 0.0f, 1.0f};
    constexpr static glm::vec3 WORLD_ORIGIN = {0.0f, 0.0f, 0.0f};

private:
    float m_FOV = glm::radians(100.0f);
    float m_Aspect = 16.0f / 9.0f;
    float m_Near = 0.1f;
    float m_Far = 1000.0f;

    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};

    glm::vec3 m_ForwardVector = {0.0f, 0.0f, -1.0f};

    glm::mat4 m_ViewMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    glm::mat4 m_ProjMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

public:
    float FOV() const { return m_FOV; }
    void SetFOV(float FOV) { m_FOV = FOV; }

    float Aspect() const { return m_Aspect; }
    void SetAspect(float Aspect) { m_Aspect = Aspect; }

    float Near() const { return m_Near; }
    void SetNear(float Near) { m_Near = Near; }

    float Far() const { return m_Far; }
    void SetFar(float Far) { m_Far = Far; }

    glm::vec3 Position() const { return m_Position; }
    void SetPosition(glm::vec3 Position) { m_Position = Position; }

    glm::mat4 ViewMatrix() const { return m_ViewMatrix; }
    void SetViewMatrix(glm::mat4 ViewMatrix) { m_ViewMatrix = ViewMatrix; }

    glm::mat4 ProjMatrix() const { return m_ProjMatrix; }
    void SetProjMatrix(glm::mat4 ProjMatrix) { m_ProjMatrix = ProjMatrix; }

    glm::vec3 ForwardVector() const { return m_ForwardVector; }
    void SetForwardVector(const glm::vec3& ForwardVector) { m_ForwardVector = ForwardVector; }
};

}
