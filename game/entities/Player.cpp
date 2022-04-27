#include "Player.h"

#include "handlers/MouseHandler.h"
#include <cmath>

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

    ProcessMovement();
    UpdateCollision();
    ProcessCollision();

    ProcessRotation();
    UpdateCamera();

    ProcessScrolls();

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
    if (MouseHandler::LeftClick())
    {
        Player::BreakBlock();
    }
    else if (MouseHandler::RightClick())
    {
        Player::PlaceBlock();
    }
}

void Player::ProcessMovement()
{
    m_LastPosition = m_Position;

    float magnitude = m_MovementSpeed * Engine::TimeDelta();
    glm::vec3 movementDirection = {0.0f, 0.0f, 0.0f};

    // Used so when walking forward vertical movement doesn't occur.
    glm::vec3 cameraFowardXZ = {m_ForwardVector.x, 0.0f, m_ForwardVector.z};

    // Sprinting
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        magnitude *= 10;
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
        movementDirection += Camera::POSITIVE_Y;
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        movementDirection -= Camera::POSITIVE_Y;
    }

    m_Position += movementDirection * magnitude;
}

void Player::ProcessRotation()
{
    // This no longer requires a delta time variable.
    // The delta x and delta y variables from the mouse handler are an
    // accumulation of movement over each frame, and this function is
    // run each frame as well, so no need to rely on a dt.
    m_Yaw += (MouseHandler::DeltaX() * m_RotationSpeed) / 300.0;
    m_Pitch += (MouseHandler::DeltaY() * m_RotationSpeed) / 300.0;

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
    m_RightVector = glm::cross(m_ForwardVector, Camera::POSITIVE_Y);
}

void Player::ProcessScrolls()
{
    newOffset = MouseHandler::ScrollOffset();

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

    oldOffset = MouseHandler::ScrollOffset();
}

void Player::UpdateCamera()
{
    Camera::SetPosition(m_Position);
    Camera::SetForwardVector(m_ForwardVector);
}

void Player::ProcessCollision()
{
    // Process movement in each direction
    glm::vec3 position = m_Position;
    glm::ivec3 lower = glm::ivec3(floor(position));
    glm::ivec3 upper = glm::ivec3(ceil(position));

    if (m_Position.x > m_LastPosition.x)
    {
        glm::ivec3 pos1 = {upper.x, upper.y, upper.z};
        glm::ivec3 pos2 = {upper.x, upper.y, lower.z};
        glm::ivec3 pos3 = {upper.x, lower.y, lower.z};
        glm::ivec3 pos4 = {upper.x, lower.y, upper.z};

        bool test1 = false;
        bool test2 = false;
        bool test3 = false;
        bool test4 = false;

        if (World::GetBlock(pos1).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos1, 1.0f, 1.0f, 1.0f);
            test1 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos2).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos2, 1.0f, 1.0f, 1.0f);
            test2 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos3).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos3, 1.0f, 1.0f, 1.0f);
            test3 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos4).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos4, 1.0f, 1.0f, 1.0f);
            test4 = Collision::Intersect(m_Collision, collision);
        }

        if (test1 || test2 || test3 || test4)
        {
            m_Position.x = m_LastPosition.x;
            UpdateCollision();
        }
    }
    else
    {
        glm::ivec3 pos1 = {lower.x, upper.y, upper.z};
        glm::ivec3 pos2 = {lower.x, upper.y, lower.z};
        glm::ivec3 pos3 = {lower.x, lower.y, lower.z};
        glm::ivec3 pos4 = {lower.x, lower.y, upper.z};

        bool test1 = false;
        bool test2 = false;
        bool test3 = false;
        bool test4 = false;

        if (World::GetBlock(pos1).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos1, 1.0f, 1.0f, 1.0f);
            test1 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos2).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos2, 1.0f, 1.0f, 1.0f);
            test2 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos3).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos3, 1.0f, 1.0f, 1.0f);
            test3 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos4).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos4, 1.0f, 1.0f, 1.0f);
            test4 = Collision::Intersect(m_Collision, collision);
        }

        if (test1 || test2 || test3 || test4)
        {
            m_Position.x = m_LastPosition.x;
            UpdateCollision();
        }
    }

    if (m_Position.y > m_LastPosition.y)
    {
        glm::ivec3 pos1 = {upper.x, upper.y, upper.z};
        glm::ivec3 pos2 = {upper.x, upper.y, lower.z};
        glm::ivec3 pos3 = {lower.x, upper.y, lower.z};
        glm::ivec3 pos4 = {lower.x, upper.y, upper.z};

        bool test1 = false;
        bool test2 = false;
        bool test3 = false;
        bool test4 = false;

        if (World::GetBlock(pos1).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos1, 1.0f, 1.0f, 1.0f);
            test1 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos2).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos2, 1.0f, 1.0f, 1.0f);
            test2 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos3).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos3, 1.0f, 1.0f, 1.0f);
            test3 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos4).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos4, 1.0f, 1.0f, 1.0f);
            test4 = Collision::Intersect(m_Collision, collision);
        }

        if (test1 || test2 || test3 || test4)
        {
            m_Position.y = m_LastPosition.y;
            UpdateCollision();
        }
    }
    else
    {
        glm::ivec3 pos1 = {upper.x, lower.y, upper.z};
        glm::ivec3 pos2 = {upper.x, lower.y, lower.z};
        glm::ivec3 pos3 = {lower.x, lower.y, lower.z};
        glm::ivec3 pos4 = {lower.x, lower.y, upper.z};

        bool test1 = false;
        bool test2 = false;
        bool test3 = false;
        bool test4 = false;

        if (World::GetBlock(pos1).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos1, 1.0f, 1.0f, 1.0f);
            test1 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos2).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos2, 1.0f, 1.0f, 1.0f);
            test2 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos3).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos3, 1.0f, 1.0f, 1.0f);
            test3 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos4).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos4, 1.0f, 1.0f, 1.0f);
            test4 = Collision::Intersect(m_Collision, collision);
        }

        if (test1 || test2 || test3 || test4)
        {
            m_Position.y = m_LastPosition.y;
            UpdateCollision();
        }
    }

    if (m_Position.z > m_LastPosition.z)
    {
        glm::ivec3 pos1 = {upper.x, upper.y, upper.z};
        glm::ivec3 pos2 = {upper.x, lower.y, upper.z};
        glm::ivec3 pos3 = {lower.x, lower.y, upper.z};
        glm::ivec3 pos4 = {lower.x, upper.y, upper.z};

        bool test1 = false;
        bool test2 = false;
        bool test3 = false;
        bool test4 = false;

        if (World::GetBlock(pos1).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos1, 1.0f, 1.0f, 1.0f);
            test1 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos2).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos2, 1.0f, 1.0f, 1.0f);
            test2 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos3).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos3, 1.0f, 1.0f, 1.0f);
            test3 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos4).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos4, 1.0f, 1.0f, 1.0f);
            test4 = Collision::Intersect(m_Collision, collision);
        }

        if (test1 || test2 || test3 || test4)
        {
            m_Position.z = m_LastPosition.z;
            UpdateCollision();
        }
    }
    else
    {
        glm::ivec3 pos1 = {upper.x, upper.y, lower.z};
        glm::ivec3 pos2 = {upper.x, lower.y, lower.z};
        glm::ivec3 pos3 = {lower.x, lower.y, lower.z};
        glm::ivec3 pos4 = {lower.x, upper.y, lower.z};

        bool test1 = false;
        bool test2 = false;
        bool test3 = false;
        bool test4 = false;

        if (World::GetBlock(pos1).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos1, 1.0f, 1.0f, 1.0f);
            test1 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos2).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos2, 1.0f, 1.0f, 1.0f);
            test2 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos3).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos3, 1.0f, 1.0f, 1.0f);
            test3 = Collision::Intersect(m_Collision, collision);
        }
        if (World::GetBlock(pos4).IsSolid)
        {
            Collision collision = Collision::CollisionCenteredXYZ(pos4, 1.0f, 1.0f, 1.0f);
            test4 = Collision::Intersect(m_Collision, collision);
        }

        if (test1 || test2 || test3 || test4)
        {
            m_Position.z = m_LastPosition.z;
            UpdateCollision();
        }
    }
}

void Player::UpdateCollision()
{
    m_Collision = {m_Position.x + (0.5f * m_Width), m_Position.x - (0.5f * m_Width), m_Position.y + (0.5f * m_Width),
                   m_Position.y - (0.5f * m_Width), m_Position.z + (0.5f * m_Width), m_Position.z - (0.5f * m_Width)};
}
