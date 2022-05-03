#include "EntityHandler.h"

void EntityHandler::Initialize() { Instance(); }

void EntityHandler::AddToUpdater(Entity *entity) { Instance().m_Updates.insert(entity); }
void EntityHandler::AddToFrameUpdater(Entity *entity) { Instance().m_FrameUpdates.insert(entity); }

void EntityHandler::RemoveFromUpdater(Entity *entity) { Instance().m_Updates.erase(entity); }
void EntityHandler::RemoveFromFrameUpdater(Entity *entity) { Instance().m_FrameUpdates.erase(entity); }

void EntityHandler::Update()
{
    for (const auto &entity : Instance().m_Updates)
    {
        entity->Update();
    }
}

void EntityHandler::FrameUpdate()
{
    for (const auto &entity : Instance().m_Updates)
    {
        entity->FrameUpdate();
    }
}
