#pragma once

#include <comet.pch>

class Camera
{
public:
    inline static auto &Instance()
    {
        static Camera instance;
        return instance;
    }

    static void Initialize();
    static void Update();
    static void CalcViewMatrix();
    static void CalcProjMatrix();

    static void Move();
    static void Rotate();

    /* World Variables */
    constexpr static glm::vec3 POSITIVE_X = {1.0f, 0.0f, 0.0f};
    constexpr static glm::vec3 POSITIVE_Y = {0.0f, 1.0f, 0.0f};
    constexpr static glm::vec3 POSITIVE_Z = {0.0f, 0.0f, 1.0f};
    constexpr static glm::vec3 WORLD_ORIGIN = {0.0f, 0.0f, 0.0f};

private:
    Camera() {}
    Camera(Camera const &);
    void operator=(Camera const &) {}

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
    static float FOV() { return Instance().m_FOV; }
    static void SetFOV(float FOV) { Instance().m_FOV = FOV; }

    static float Aspect() { return Instance().m_Aspect; }
    static void SetAspect(float Aspect) { Instance().m_Aspect = Aspect; }

    static float Near() { return Instance().m_Near; }
    static void SetNear(float Near) { Instance().m_Near = Near; }

    static float Far() { return Instance().m_Far; }
    static void SetFar(float Far) { Instance().m_Far = Far; }

    static glm::vec3 Position() { return Instance().m_Position; }
    static void SetPosition(glm::vec3 Position) { Instance().m_Position = Position; }

    static glm::mat4 ViewMatrix() { return Instance().m_ViewMatrix; }
    static void SetViewMatrix(glm::mat4 ViewMatrix) { Instance().m_ViewMatrix = ViewMatrix; }

    static glm::mat4 ProjMatrix() { return Instance().m_ProjMatrix; }
    static void SetProjMatrix(glm::mat4 ProjMatrix) { Instance().m_ProjMatrix = ProjMatrix; }

    static glm::vec3 ForwardVector() { return Instance().m_ForwardVector; }
    static void SetForwardVector(const glm::vec3 &ForwardVector) { Instance().m_ForwardVector = ForwardVector; }
};
