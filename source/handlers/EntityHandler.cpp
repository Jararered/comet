#include "EntityHandler.h"

EntityHandler::EntityHandler() {}

EntityHandler::~EntityHandler() {}

void EntityHandler::AddToUpdater(Entity *entity) { m_Updates.insert(entity); }
void EntityHandler::AddToFrameUpdater(Entity *entity) { m_FrameUpdates.insert(entity); }

void EntityHandler::RemoveFromUpdater(Entity *entity) { m_Updates.erase(entity); }
void EntityHandler::RemoveFromFrameUpdater(Entity *entity) { m_FrameUpdates.erase(entity); }

void EntityHandler::Update()
{
    for (const auto &entity : m_Updates)
    {
        entity->Update();
    }
}

void EntityHandler::FrameUpdate()
{
    for (const auto &entity : m_Updates)
    {
        entity->FrameUpdate();
    }
}
