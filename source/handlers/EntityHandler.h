#pragma once

#include <comet.pch>

#include "Entity.h"

class EntityHandler
{
public:
    EntityHandler();
    ~EntityHandler();

    void AddToUpdater(Entity *entity);
    void RemoveFromUpdater(Entity *entity);

    void AddToFrameUpdater(Entity *entity);
    void RemoveFromFrameUpdater(Entity *entity);

    // Should be called from the game loop, not the engine loop
    void Update();
    // Should be called from the engine loop, not the game loop
    void FrameUpdate();

private:
    std::unordered_set<Entity *> m_Updates;
    std::unordered_set<Entity *> m_FrameUpdates;
};
