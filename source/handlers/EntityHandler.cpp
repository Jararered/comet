#include "EntityHandler.h"

void EntityHandler::Initialize() { Get(); }

void EntityHandler::AddToUpdater(Entity *entity) { Get().m_Updates.insert(entity); }
void EntityHandler::AddToFrameUpdater(Entity *entity) { Get().m_FrameUpdates.insert(entity); }

void EntityHandler::RemoveFromUpdater(Entity *entity) { Get().m_Updates.erase(entity); }
void EntityHandler::RemoveFromFrameUpdater(Entity *entity) { Get().m_FrameUpdates.erase(entity); }

void EntityHandler::Update()
{
    for (const auto &entity : Get().m_Updates)
    {
        entity->Update();
    }
}

void EntityHandler::FrameUpdate()
{
    for (const auto &entity : Get().m_Updates)
    {
        entity->FrameUpdate();
    }
}
