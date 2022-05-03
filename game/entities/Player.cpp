#include "Player.h"

#include "handlers/MouseHandler.h"
#include <cmath>

Player::Player()
{
    m_Camera->SetPosition(m_Position);
    m_EntityHandler->AddToUpdater(this);
    m_EntityHandler->AddToFrameUpdater(this);
}

Player::~Player() {}

void Player::Update() { GetRequestedChunks(); }

void Player::FrameUpdate()
{
    ProcessClicks();

    ProcessMovement();
    UpdateBoundingBox();
    ProcessCollision();

    ProcessRotation();
    UpdateCamera();

    ProcessScrolls();

    Block blockInsideOf = World::GetBlock(round(m_Position));
    if (blockInsideOf.ID == ID::Water && blockInsideOf.ID != m_LastBlockInsideOf.ID)
    {
        SetInWater(true);
        m_Renderer->SetOverlayColor({-0.1f, -0.1f, 0.3f});
    }

    if (blockInsideOf.ID != ID::Water && m_LastBlockInsideOf.ID == ID::Water)
    {
        SetInWater(false);
        m_Renderer->SetOverlayColor({0.0f, 0.0f, 0.0f});
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
    glm::vec3 direction = m_Direction;
    glm::vec3 position = m_Position;
    glm::vec3 positionLast = m_Position;

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

            World::SetBlock(round(positionLast), Blocks::GetBlockFromID(m_SelectedBlock));
            return;
        }
        positionLast = position + direction;
        first = false;
    }
}

void Player::BreakBlock()
{
    float step = 1.0f / 16.0f;
    glm::vec3 direction = m_Direction;
    glm::vec3 position = m_Position;

    while (glm::length(direction) < 5.0f)
    {
        direction += glm::normalize(direction) * step;
        if (World::GetBlock(round(position + direction)).ID != 0)
        {
            World::SetBlock(round(position + direction), Blocks::Air());
            return;
        }
    }
}

void Player::ProcessClicks()
{
    if (m_MouseHandler->LeftClick())
    {
        Player::BreakBlock();
    }
    else if (m_MouseHandler->RightClick())
    {
        Player::PlaceBlock();
    }
}

void Player::ProcessMovement()
{
    m_LastPosition = m_Position;

    float magnitude = m_MovementSpeed * m_Engine->TimeDelta();
    glm::vec3 movementDirection = {0.0f, 0.0f, 0.0f};

    // Used so when walking forward vertical movement doesn't occur.
    glm::vec3 cameraFowardXZ = {m_ForwardVector.x, 0.0f, m_ForwardVector.z};

    // Sprinting
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        magnitude *= 3;
    }

    // Basic movement processing
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
    {
        movementDirection += cameraFowardXZ;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
    {
        movementDirection -= cameraFowardXZ;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
    {
        movementDirection -= m_RightVector;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
    {
        movementDirection += m_RightVector;
    }

    // Fixes diagonal directed movement to not be faster than along an axis.
    // Only happens when holding two buttons that are off axis from each other.
    if (movementDirection.x != 0.0f || movementDirection.y != 0.0f)
    {
        movementDirection = glm::normalize(movementDirection);
    }

    // Still perform up/down movements after normalization.
    // Don't care about limiting speed along verticals.
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        movementDirection += m_Camera->POSITIVE_Y;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        movementDirection -= m_Camera->POSITIVE_Y;
    }

    m_Position += movementDirection * magnitude;
}

void Player::ProcessRotation()
{
    // This no longer requires a delta time variable.
    // The delta x and delta y variables from the mouse handler are an
    // accumulation of movement over each frame, and this function is
    // run each frame as well, so no need to rely on a dt.
    m_Yaw += (m_MouseHandler->DeltaX() * m_RotationSpeed) / 300.0;
    m_Pitch += (m_MouseHandler->DeltaY() * m_RotationSpeed) / 300.0;

    // Keep yaw angle from getting to imprecise
    if (m_Yaw > glm::radians(360.0f))
    {
        m_Yaw -= glm::radians(360.0f);
    }
    else if (m_Yaw < glm::radians(-360.0f))
    {
        m_Yaw += glm::radians(360.0f);
    }

    // Keep pitch angle from going too far over
    if (m_Pitch > glm::radians(89.0f))
    {
        m_Pitch = glm::radians(89.0f);
    }
    else if (m_Pitch < glm::radians(-89.0f))
    {
        m_Pitch = glm::radians(-89.0f);
    }

    m_Direction.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
    m_Direction.y = glm::sin(m_Pitch);
    m_Direction.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);

    m_ForwardVector = glm::normalize(m_Direction);
    m_RightVector = glm::cross(m_ForwardVector, m_Camera->POSITIVE_Y);
}

void Player::ProcessScrolls()
{
    newOffset = m_MouseHandler->ScrollOffset();

    if (newOffset > oldOffset)
    {
        m_SelectedBlock = m_SelectedBlock + 1;
        std::cout << static_cast<int>(m_SelectedBlock) << std::endl;
    }

    if (newOffset < oldOffset)
    {
        m_SelectedBlock = m_SelectedBlock - 1;
        std::cout << static_cast<int>(m_SelectedBlock) << std::endl;
    }

    oldOffset = m_MouseHandler->ScrollOffset();
}

void Player::UpdateCamera()
{
    m_Camera->SetPosition(m_Position);
    m_Camera->SetForwardVector(m_ForwardVector);
}

void Player::ProcessCollision()
{
    // Process movement in each direction
    CheckYCollision();
    CheckXCollision();
    CheckZCollision();
}

void Player::UpdateBoundingBox()
{
    m_BoundingBox = {m_Position.x + (0.5f * m_Width), m_Position.x - (0.5f * m_Width), m_Position.y + 0.25f,
                     m_Position.y - m_Height,         m_Position.z + (0.5f * m_Width), m_Position.z - (0.5f * m_Width)};
}

void Player::CheckXCollision()
{
    // Process movement in each direction
    glm::ivec3 lower = glm::ivec3(floor(m_Position));
    glm::ivec3 upper = glm::ivec3(ceil(m_Position));

    std::vector<glm::ivec3> positions;
    std::vector<bool> tests;
    Collision collision;

    if (m_Position.x > m_LastPosition.x)
    {
        positions = {{upper.x, upper.y, upper.z}, {upper.x, upper.y, lower.z},     {upper.x, lower.y, lower.z},
                     {upper.x, lower.y, upper.z}, {upper.x, lower.y - 1, lower.z}, {upper.x, lower.y - 1, upper.z}};
    }
    else
    {
        positions = {{lower.x, upper.y, upper.z}, {lower.x, upper.y, lower.z},     {lower.x, lower.y, lower.z},
                     {lower.x, lower.y, upper.z}, {lower.x, lower.y - 1, lower.z}, {lower.x, lower.y - 1, upper.z}};
    }
    tests = {false, false, false, false, false, false};
    for (int i = 0; i < 6; i++)
    {
        if (World::GetBlock(positions[i]).IsSolid)
        {
            collision = Collision::CollisionCenteredXYZ(positions[i], 1.0f, 1.0f, 1.0f);
            tests[i] = Collision::Intersect(m_BoundingBox, collision);
        }
    }
    if (tests[0] || tests[1] || tests[2] || tests[3] || tests[4] || tests[5])
    {
        m_Position.x = m_LastPosition.x;
        UpdateBoundingBox();
    }
}

void Player::CheckYCollision()
{
    // Process movement in each direction
    glm::ivec3 lower = glm::ivec3(floor(m_Position - glm::vec3(0.0f, m_Height, 0.0f)));
    glm::ivec3 upper = glm::ivec3(ceil(m_Position));

    std::vector<glm::ivec3> positions;
    std::vector<bool> tests;
    Collision collision;

    // Y Processing
    if (m_Position.y > m_LastPosition.y)
    {
        positions = {{upper.x, upper.y, upper.z},
                     {upper.x, upper.y, lower.z},
                     {lower.x, upper.y, lower.z},
                     {lower.x, upper.y, upper.z}};
    }
    else
    {
        positions = {{upper.x, lower.y, upper.z},
                     {upper.x, lower.y, lower.z},
                     {lower.x, lower.y, lower.z},
                     {lower.x, lower.y, upper.z}};
    }
    tests = {false, false, false, false};
    for (int i = 0; i < 4; i++)
    {
        if (World::GetBlock(positions[i]).IsSolid)
        {
            collision = Collision::CollisionCenteredXYZ(positions[i], 1.0f, 1.0f, 1.0f);
            tests[i] = Collision::Intersect(m_BoundingBox, collision);
        }
    }
    if (tests[0] || tests[1] || tests[2] || tests[3])
    {
        m_Position.y = m_LastPosition.y;
        UpdateBoundingBox();
    }
}

void Player::CheckZCollision()
{
    // Process movement in each direction
    glm::ivec3 lower = glm::ivec3(floor(m_Position));
    glm::ivec3 upper = glm::ivec3(ceil(m_Position));

    std::vector<glm::ivec3> positions;
    std::vector<bool> tests;
    Collision collision;

    // Z Processing
    if (m_Position.z > m_LastPosition.z)
    {
        positions = {{upper.x, upper.y, upper.z}, {lower.x, upper.y, upper.z},     {lower.x, lower.y, upper.z},
                     {upper.x, lower.y, upper.z}, {lower.x, lower.y - 1, upper.z}, {upper.x, lower.y - 1, upper.z}};
    }
    else
    {
        positions = {{upper.x, upper.y, lower.z}, {lower.x, upper.y, lower.z},     {lower.x, lower.y, lower.z},
                     {upper.x, lower.y, lower.z}, {lower.x, lower.y - 1, lower.z}, {upper.x, lower.y - 1, lower.z}};
    }
    tests = {false, false, false, false, false, false};
    for (int i = 0; i < 6; i++)
    {
        if (World::GetBlock(positions[i]).IsSolid)
        {
            collision = Collision::CollisionCenteredXYZ(positions[i], 1.0f, 1.0f, 1.0f);
            tests[i] = Collision::Intersect(m_BoundingBox, collision);
        }
    }
    if (tests[0] || tests[1] || tests[2] || tests[3] || tests[4] || tests[5])
    {
        m_Position.z = m_LastPosition.z;
        UpdateBoundingBox();
    }
}
