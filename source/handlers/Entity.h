#pragma once

#include <comet.pch>
#include <stdint.h>

class Entity
{
public:
    Entity() {}
    ~Entity() {}

    // Updated on the main thread
    // GLFW calls are valid here
    virtual void FrameUpdate() {}

    // Updated on separate threads
    // Used for user-made threads
    virtual void Update() {}

protected:
    glm::vec3 m_Position;
    glm::vec3 m_LastPosition;

    glm::vec3 m_Velocity;
    glm::vec3 m_LastVelocity;

    glm::vec3 m_Acceleration;
    glm::vec3 m_LastAcceleration;

public:
    glm::vec3 Position() const { return m_Position; }
    void SetPosition(const glm::vec3 &Position) { m_Position = Position; }

    glm::vec3 LastPosition() const { return m_LastPosition; }
    void SetLastPosition(const glm::vec3 &LastPosition) { m_LastPosition = LastPosition; }

    glm::vec3 Velocity() const { return m_Velocity; }
    void SetVelocity(const glm::vec3 &Velocity) { m_Velocity = Velocity; }

    glm::vec3 LastVelocity() const { return m_LastVelocity; }
    void SetLastVelocity(const glm::vec3 &LastVelocity) { m_LastVelocity = LastVelocity; }

    glm::vec3 Acceleration() const { return m_Acceleration; }
    void SetAcceleration(const glm::vec3 &Acceleration) { m_Acceleration = Acceleration; }

    glm::vec3 LastAcceleration() const { return m_LastAcceleration; }
    void SetLastAcceleration(const glm::vec3 &LastAcceleration) { m_LastAcceleration = LastAcceleration; }
};
