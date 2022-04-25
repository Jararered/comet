#include "Player.h"
#include "handlers/MouseHandler.h"

Player::Player()
{
    Camera::SetPosition(m_Position);
    EntityHandler::AddToUpdater(this);
    EntityHandler::AddToFrameUpdater(this);
}

Player::~Player() {}

void Player::Update() { GetRequestedChunks(); }

void Player::FrameUpdate()
{
    ProcessClicks();
    ProcessKeys();
    ProcessScrolls();

    m_Position = Camera::Position();
    Block blockInsideOf = World::GetBlock(round(m_Position));
    if (blockInsideOf.ID == ID::Water && blockInsideOf.ID != m_LastBlockInsideOf.ID)
    {
        SetInWater(true);
        Renderer::SetOverlayColor({-0.1f, -0.1f, 0.3f});
    }

    if (blockInsideOf.ID != ID::Water && m_LastBlockInsideOf.ID == ID::Water)
    {
        SetInWater(false);
        Renderer::SetOverlayColor({0.0f, 0.0f, 0.0f});
    }

    m_LastBlockInsideOf = blockInsideOf;
}

void Player::GetRequestedChunks()
{
    glm::ivec3 newChunkIndex = World::GetChunkIndex(m_Position);

    World::ProcessRequestedChunks(newChunkIndex);
}

void Player::PlaceBlock()
{
    float step = 1.0f / 16.0f;
    glm::vec3 direction = Camera::Direction();
    glm::vec3 position = Camera::Position();
    glm::vec3 positionLast = position;

    bool first = true;

    while (glm::length(direction) < 5.0f)
    {
        direction += glm::normalize(direction) * step;
        if (World::GetBlock(round(position + direction)).ID != 0)
        {
            // If there is a block on the first check, player is either
            // inside of a block or way too close.
            if (first)
            {
                return;
            }

            World::SetBlock(round(positionLast), m_SelectedBlock);
            return;
        }
        positionLast = position + direction;
        first = false;
    }
}

void Player::BreakBlock()
{
    float step = 1.0f / 16.0f;
    glm::vec3 direction = Camera::Direction();
    glm::vec3 position = Camera::Position();

    while (glm::length(direction) < 5.0f)
    {
        direction += glm::normalize(direction) * step;
        if (World::GetBlock(round(position + direction)).ID != 0)
        {
            World::SetBlock(round(position + direction), Block(0, true));
            return;
        }
    }
}

void Player::ProcessClicks()
{
    if (MouseHandler::LeftClick())
    {
        Player::BreakBlock();
    }
    else if (MouseHandler::RightClick())
    {
        Player::PlaceBlock();
    }
}

void Player::ProcessKeys()
{
    if (glfwGetKey(WindowHandler::Window(), GLFW_KEY_HOME))
    {
        Camera::SetPosition({0.0f, 100.0f, 0.0f});
    }
}

void Player::ProcessScrolls()
{
    newOffset = MouseHandler::ScrollOffset();

    // if (newOffset > oldOffset)
    // {
    //     m_SelectedBlock = Block(m_SelectedBlock.ID + 1);
    //     std::cout << static_cast<int>(m_SelectedBlock.ID) << std::endl;
    // }

    // if (newOffset < oldOffset)
    // {
    //     m_SelectedBlock = Block(m_SelectedBlock.ID - 1);
    //     std::cout << static_cast<int>(m_SelectedBlock.ID) << std::endl;
    // }

    oldOffset = MouseHandler::ScrollOffset();
}
