#pragma once

#include <comet.pch>
#include <stdint.h>

class Entity
{
public:
    Entity() {}
    ~Entity() {}

    virtual void Update() {}
    virtual void FrameUpdate() {}

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
};
