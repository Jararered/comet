#pragma once

#include <comet.pch>

#include "Entity.h"

class EntityHandler
{
public:
    inline static auto &Get()
    {
        static EntityHandler instance;
        return instance;
    }

    static void Initialize();

    static void AddToUpdater(Entity *entity);
    static void RemoveFromUpdater(Entity *entity);

    static void AddToFrameUpdater(Entity *entity);
    static void RemoveFromFrameUpdater(Entity *entity);

    // Should be called from the game loop, not the engine loop
    static void Update();
    // Should be called from the engine loop, not the game loop
    static void FrameUpdate();

private:
    EntityHandler() {}
    EntityHandler(EntityHandler const &);
    void operator=(EntityHandler const &);

    std::unordered_set<Entity *> m_Updates;
    std::unordered_set<Entity *> m_FrameUpdates;
};
