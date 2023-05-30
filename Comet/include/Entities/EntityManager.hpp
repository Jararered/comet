#pragma once

#include "Entity.hpp"

#include <unordered_set>

class EntityManager
{
public:
    inline static auto& Get()
    {
        static EntityManager instance;
        return instance;
    }

    static void Initialize() { Get(); }

    static void AddToUpdater(Entity* entity) { Get().m_Updates.insert(entity); }
    static void AddToFrameUpdater(Entity* entity) { Get().m_FrameUpdates.insert(entity); }

    static void RemoveFromUpdater(Entity* entity) { Get().m_Updates.erase(entity); }
    static void RemoveFromFrameUpdater(Entity* entity) { Get().m_FrameUpdates.erase(entity); }

    // Should be called from the engine loop, not the game loop
    static void Update()
    {
        for (const auto& entity : Get().m_Updates)
        {
            entity->Update();
        }
    }

    // Should be called from the game loop, not the engine loop
    static void FrameUpdate(float dt)
    {
        for (const auto& entity : Get().m_FrameUpdates)
        {
            entity->FrameUpdate(dt);
        }
    }

private:
    EntityManager() {}
    EntityManager(EntityManager const&);
    void operator=(EntityManager const&);

    std::unordered_set<Entity*> m_Updates;
    std::unordered_set<Entity*> m_FrameUpdates;
};
