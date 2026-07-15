#pragma once

#include "Entity.h"

#include <mutex>
#include <unordered_set>
#include <vector>

class EntityManager
{
public:
    void AddToUpdater(Entity* entity)
    {
        std::lock_guard lock(m_Mutex);
        m_Updates.insert(entity);
    }

    void AddToFrameUpdater(Entity* entity)
    {
        std::lock_guard lock(m_Mutex);
        m_FrameUpdates.insert(entity);
    }

    void AddToPhysicsUpdater(Entity* entity)
    {
        std::lock_guard lock(m_Mutex);
        m_PhysicsUpdates.insert(entity);
    }

    void RemoveFromUpdater(Entity* entity)
    {
        std::lock_guard lock(m_Mutex);
        m_Updates.erase(entity);
    }

    void RemoveFromFrameUpdater(Entity* entity)
    {
        std::lock_guard lock(m_Mutex);
        m_FrameUpdates.erase(entity);
    }

    void RemoveFromPhysicsUpdater(Entity* entity)
    {
        std::lock_guard lock(m_Mutex);
        m_PhysicsUpdates.erase(entity);
    }

    // Should be called from the engine loop, not the game loop
    void Update()
    {
        for (const auto& entity : Snapshot(m_Updates))
        {
            entity->Update();
        }
    }

    // Should be called from the game loop, not the engine loop
    void FrameUpdate(float dt)
    {
        for (const auto& entity : Snapshot(m_FrameUpdates))
        {
            entity->FrameUpdate(dt);
        }
    }

    void PhysicsUpdate(float dt)
    {
        for (const auto& entity : Snapshot(m_PhysicsUpdates))
        {
            entity->PhysicsUpdate(dt);
        }
    }

private:
    std::vector<Entity*> Snapshot(const std::unordered_set<Entity*>& entities)
    {
        std::lock_guard lock(m_Mutex);
        return {entities.begin(), entities.end()};
    }

    std::mutex m_Mutex;
    std::unordered_set<Entity*> m_Updates;
    std::unordered_set<Entity*> m_FrameUpdates;
    std::unordered_set<Entity*> m_PhysicsUpdates;
};
