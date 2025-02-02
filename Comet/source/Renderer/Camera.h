#pragma once

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

class Camera
{
public:
    inline static auto& Get()
    {
        static Camera instance;
        return instance;
    }

    static void Initialize();
    static void Update();
    static void CalcViewMatrix();
    static void CalcProjMatrix();

    /* World Variables */
    constexpr static glm::vec3 POSITIVE_X = {1.0f, 0.0f, 0.0f};
    constexpr static glm::vec3 POSITIVE_Y = {0.0f, 1.0f, 0.0f};
    constexpr static glm::vec3 POSITIVE_Z = {0.0f, 0.0f, 1.0f};
    constexpr static glm::vec3 WORLD_ORIGIN = {0.0f, 0.0f, 0.0f};

private:
    Camera() {}
    Camera(Camera const&);
    void operator=(Camera const&) {}

    float m_FOV = glm::radians(100.0f);
    float m_Aspect = 16.0f / 9.0f;
    float m_Near = 0.1f;
    float m_Far = 1000.0f;

    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};

    glm::vec3 m_ForwardVector = {0.0f, 0.0f, -1.0f};

    glm::mat4 m_ViewMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    glm::mat4 m_ProjMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

public:
    static float FOV() { return Get().m_FOV; }
    static void SetFOV(float FOV) { Get().m_FOV = FOV; }

    static float Aspect() { return Get().m_Aspect; }
    static void SetAspect(float Aspect) { Get().m_Aspect = Aspect; }

    static float Near() { return Get().m_Near; }
    static void SetNear(float Near) { Get().m_Near = Near; }

    static float Far() { return Get().m_Far; }
    static void SetFar(float Far) { Get().m_Far = Far; }

    static glm::vec3 Position() { return Get().m_Position; }
    static void SetPosition(glm::vec3 Position) { Get().m_Position = Position; }

    static glm::mat4 ViewMatrix() { return Get().m_ViewMatrix; }
    static void SetViewMatrix(glm::mat4 ViewMatrix) { Get().m_ViewMatrix = ViewMatrix; }

    static glm::mat4 ProjMatrix() { return Get().m_ProjMatrix; }
    static void SetProjMatrix(glm::mat4 ProjMatrix) { Get().m_ProjMatrix = ProjMatrix; }

    static glm::vec3 ForwardVector() { return Get().m_ForwardVector; }
    static void SetForwardVector(const glm::vec3& ForwardVector) { Get().m_ForwardVector = ForwardVector; }
};
