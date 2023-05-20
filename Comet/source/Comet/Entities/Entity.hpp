#pragma once

#include <glm/vec3.hpp>

struct Entity
{
    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_LastPosition = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Acceleration = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_Gravity = { 0.0f, 0.0f, 0.0f };

    void SetPosition(glm::vec3 newPosition)
    {
        m_Position = newPosition;
        m_LastPosition = newPosition;
    }

    // Updated on the main thread
    // GLFW calls are valid here
    virtual void FrameUpdate(float dt)
    {
        ApplyGravity();
        UpdatePosition(dt);
    }

    // Updated on separate threads
    // Used for user-made threads
    virtual void Update() {}

    virtual void ApplyGravity() { m_Acceleration += m_Gravity; }

    virtual void ApplyAcceleration(glm::vec3 acceleration) { m_Acceleration += acceleration; }

    virtual void ApplyMovement(glm::vec3 positionChange)
    {
        m_Position += positionChange;
        m_LastPosition += positionChange;
    }

    virtual void UpdatePosition(float dt)
    {
        const glm::vec3 Velocity = m_Position - m_LastPosition;

        m_LastPosition = m_Position;

        m_Position = m_LastPosition + Velocity + m_Acceleration * dt * dt;

        m_Acceleration = { 0.0, 0.0, 0.0 };
    }
};
