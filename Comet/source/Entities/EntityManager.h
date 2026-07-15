#pragma once

#include "Entity.h"

#include <unordered_set>

class EntityManager
{
public:
    void AddToUpdater(Entity* entity) { m_Updates.insert(entity); }
    void AddToFrameUpdater(Entity* entity) { m_FrameUpdates.insert(entity); }

    void RemoveFromUpdater(Entity* entity) { m_Updates.erase(entity); }
    void RemoveFromFrameUpdater(Entity* entity) { m_FrameUpdates.erase(entity); }

    // Should be called from the engine loop, not the game loop
    void Update()
    {
        for (const auto& entity : m_Updates)
        {
            entity->Update();
        }
    }

    // Should be called from the game loop, not the engine loop
    void FrameUpdate(float dt)
    {
        for (const auto& entity : m_FrameUpdates)
        {
            entity->FrameUpdate(dt);
        }
    }

private:
    std::unordered_set<Entity*> m_Updates;
    std::unordered_set<Entity*> m_FrameUpdates;
};
