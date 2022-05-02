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
    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_LastPosition = {0.0f, 0.0f, 0.0f};

public:
    glm::vec3 Position() const { return m_Position; }
    void SetPosition(const glm::vec3 &Position) { m_Position = Position; }

    glm::vec3 LastPosition() const { return m_LastPosition; }
    void SetLastPosition(const glm::vec3 &LastPosition) { m_LastPosition = LastPosition; }
};
