#include "Player.h"

#include "handlers/Input.h"
#include "handlers/KeyboardKeyCodes.h"
#include "handlers/MouseButtonCodes.h"
#include "physics/Gravity.h"
#include <cmath>

using namespace Comet;

Player::Player()
{
    m_Position = {0.0f, 50.0f, 0.0f};
    m_LastPosition = {0.0f, 50.0f, 0.0f};

    m_Velocity = {0.0f, 0.0f, 0.0f};
    m_LastVelocity = {0.0f, 0.0f, 0.0f};

    m_Acceleration = {0.0f, 0.0f, 0.0f};
    m_LastAcceleration = {0.0f, 0.0f, 0.0f};

    m_GravityVel = {0.0f, 0.0f, 0.0f};

    m_Flying = true;

    Camera::SetPosition(m_Position);
    EntityHandler::AddToUpdater(this);
    EntityHandler::AddToFrameUpdater(this);
}

Player::~Player() {}

void Player::Update() { GetRequestedChunks(); }

void Player::FrameUpdate()
{
    if (Input::IsLeftClick() && !m_BreakingBlock)
    {
        m_BreakingBlock = true;
        Player::BreakBlock();
    }
    if (!Input::IsLeftClick())
    {
        m_BreakingBlock = false;
    }

    if (Input::IsRightClick() && !m_PlacingBlock)
    {
        m_PlacingBlock = true;
        Player::PlaceBlock();
    }
    if (!Input::IsRightClick())
    {
        m_PlacingBlock = false;
    }

    // Update movement and collision
    ProcessMovement();
    UpdateBoundingBox();
    ProcessCollision();

    // Update camera movement
    ProcessRotation();
    UpdateCamera();

    // Update user input/selection
    ProcessScrolls();

    // Update overlays if inside of a block
    Block blockInsideOf = World::GetBlock(round(m_Position));
    if (blockInsideOf.ID == ID::Water && blockInsideOf.ID != m_LastBlockInsideOf.ID)
    {
        m_InWater = true;
        Renderer::SetOverlayColor({-0.1f, -0.1f, 0.3f});
    }

    if (blockInsideOf.ID != ID::Water && m_LastBlockInsideOf.ID == ID::Water)
    {
        m_InWater = false;
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

void Player::ProcessMovement()
{
    float dt = static_cast<float>(Engine::TimeDelta());

    // Checks for first frame and if a lag spike occurs
    if (dt < 0.0001f || dt > 0.25f)
        return;

    // Setting last values
    m_LastAcceleration = 1 / dt * (m_LastVelocity - m_Velocity);
    m_LastVelocity = 1 / dt * (m_Position - m_LastPosition);
    m_LastPosition = m_Position;

    // Basic movement processing
    // Used so when walking forward vertical movement doesn't occur.
    float movementSpeed = m_MovementSpeed;
    glm::vec3 direction = {0.0f, 0.0f, 0.0f};
    glm::vec3 cameraFowardXZ = {m_ForwardVector.x, 0.0f, m_ForwardVector.z};
    if (Input::IsKeyPressed(CT_KEY_LEFT_CONTROL))
        movementSpeed *= 3;
    if (Input::IsKeyPressed(CT_KEY_W))
        direction += cameraFowardXZ;
    if (Input::IsKeyPressed(CT_KEY_S))
        direction -= cameraFowardXZ;
    if (Input::IsKeyPressed(CT_KEY_A))
        direction -= m_RightVector;
    if (Input::IsKeyPressed(CT_KEY_D))
        direction += m_RightVector;

    // Fixes diagonal directed movement to not be faster than along an axis.
    // Only happens when holding two buttons that are off axis from each other.
    if (direction.x != 0.0f || direction.y != 0.0f)
        direction = glm::normalize(direction);

    // Still perform up/down movements after normalization.
    // Don't care about limiting speed along verticals.
    if (Input::IsKeyPressed(CT_KEY_LEFT_SHIFT))
        direction -= Camera::POSITIVE_Y;

    if (m_Flying)
    {
        if (Input::IsKeyPressed(CT_KEY_SPACE))
            direction += Camera::POSITIVE_Y;

        m_Acceleration = {0.0f, 0.0f, 0.0f};
        m_Velocity = dt * (0.5f * m_LastAcceleration + m_Acceleration) + movementSpeed * direction;
        m_Position = dt * (0.5f * m_LastVelocity + m_Velocity) + m_LastPosition;
    }
    else
    {
        if (Input::IsKeyPressed(CT_KEY_SPACE) && m_Standing)
        {
            m_Velocity += glm::vec3{0.0f, 8.5f, 0.0f};
            std::cout << "jumping" << std::endl;
        }

        m_Velocity += m_GravityVel + dt * 2.5f * SimpleGravity;
        m_Position = dt * m_Velocity + m_LastPosition + dt * movementSpeed * direction;
    }
}

void Player::ProcessRotation()
{
    // This no longer requires a delta time variable.
    // The delta x and delta y variables from the mouse handler are an
    // accumulation of movement over each frame, and this function is
    // run each frame as well, so no need to rely on a dt.
    glm::vec2 mouseMovement = Input::GetMouseMovement();
    m_Yaw += (mouseMovement.x * m_RotationSpeed) / 300.0;
    m_Pitch += (mouseMovement.y * m_RotationSpeed) / 300.0;

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
    newOffset = Input::GetScrollOffset();

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

    oldOffset = newOffset;
}

void Player::UpdateCamera()
{
    Camera::SetPosition(m_Position);
    Camera::SetForwardVector(m_ForwardVector);
}

void Player::ProcessCollision()
{
    glm::vec3 attemptedPosition = m_Position;

    if (m_LastPosition != m_Position)
    {
        // Process movement in each direction
        CheckYCollision();
        CheckXCollision();
        CheckZCollision();

        if (m_LastPosition == m_Position)
        {
            m_Position = attemptedPosition;
            UpdateBoundingBox();

            // Process movement in each direction
            CheckYCollision();
            CheckZCollision();
            CheckXCollision();
        }
    }
}

void Player::UpdateBoundingBox()
{
    m_BoundingBox = {m_Position.x + (0.5f * m_Width), m_Position.x - (0.5f * m_Width), m_Position.y + 0.25f,
        m_Position.y - m_Height, m_Position.z + (0.5f * m_Width), m_Position.z - (0.5f * m_Width)};
}

void Player::CheckXCollision()
{
    // Process movement in each direction
    glm::ivec3 lower = glm::ivec3(floor(m_Position));
    glm::ivec3 upper = glm::ivec3(ceil(m_Position));

    std::vector<glm::ivec3> positions;
    std::vector<bool> tests;
    Collision blockBoundingBox;

    if (m_Position.x > m_LastPosition.x)
    {
        positions = {{upper.x, upper.y, upper.z}, {upper.x, upper.y, lower.z}, {upper.x, lower.y, lower.z},
            {upper.x, lower.y, upper.z}, {upper.x, lower.y - 1, lower.z}, {upper.x, lower.y - 1, upper.z}};
    }
    else
    {
        positions = {{lower.x, upper.y, upper.z}, {lower.x, upper.y, lower.z}, {lower.x, lower.y, lower.z},
            {lower.x, lower.y, upper.z}, {lower.x, lower.y - 1, lower.z}, {lower.x, lower.y - 1, upper.z}};
    }
    tests = {false, false, false, false, false, false};
    for (int i = 0; i < 6; i++)
    {
        if (World::GetBlock(positions[i]).IsSolid)
        {
            blockBoundingBox = Collision::BoundingBoxCentered(positions[i], 1.0f, 1.0f, 1.0f);
            tests[i] = Collision::IsIntersect(m_BoundingBox, blockBoundingBox);
        }
    }
    if (tests[0] || tests[1] || tests[2] || tests[3] || tests[4] || tests[5])
    {
        m_Position.x = m_LastPosition.x;
        m_Velocity.x = 0.0f;
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
    Collision blockBoundingBox;
    bool bottomCollision;

    // Y Processing
    if (m_Position.y > m_LastPosition.y)
    {
        bottomCollision = false;
        m_Standing = false;
        positions = {{upper.x, upper.y, upper.z}, {upper.x, upper.y, lower.z}, {lower.x, upper.y, lower.z},
            {lower.x, upper.y, upper.z}};
    }
    else
    {
        bottomCollision = true;
        positions = {{upper.x, lower.y, upper.z}, {upper.x, lower.y, lower.z}, {lower.x, lower.y, lower.z},
            {lower.x, lower.y, upper.z}};
    }
    tests = {false, false, false, false};
    for (int i = 0; i < 4; i++)
    {
        if (World::GetBlock(positions[i]).IsSolid)
        {
            blockBoundingBox = Collision::BoundingBoxCentered(positions[i], 1.0f, 1.0f, 1.0f);
            tests[i] = Collision::IsIntersect(m_BoundingBox, blockBoundingBox);
        }
    }
    if (tests[0] || tests[1] || tests[2] || tests[3])
    {
        m_Position.y = m_LastPosition.y;
        m_Velocity.y = 0.0f;
        UpdateBoundingBox();
    }

    // Checking if colliding with a block beneath player
    if (bottomCollision)
    {
        if (tests[0] || tests[1] || tests[2] || tests[3])
        {
            m_Standing = true;
            m_GravityVel = {0.0f, 0.0f, 0.0f};
        }
        else
        {
            m_Standing = false;
        }
    }
}

void Player::CheckZCollision()
{
    // Process movement in each direction
    glm::ivec3 lower = glm::ivec3(floor(m_Position));
    glm::ivec3 upper = glm::ivec3(ceil(m_Position));

    std::vector<glm::ivec3> positions;
    std::vector<bool> tests;
    Collision blockBoundingBox;

    // Z Processing
    if (m_Position.z > m_LastPosition.z)
    {
        positions = {{upper.x, upper.y, upper.z}, {lower.x, upper.y, upper.z}, {lower.x, lower.y, upper.z},
            {upper.x, lower.y, upper.z}, {lower.x, lower.y - 1, upper.z}, {upper.x, lower.y - 1, upper.z}};
    }
    else
    {
        positions = {{upper.x, upper.y, lower.z}, {lower.x, upper.y, lower.z}, {lower.x, lower.y, lower.z},
            {upper.x, lower.y, lower.z}, {lower.x, lower.y - 1, lower.z}, {upper.x, lower.y - 1, lower.z}};
    }
    tests = {false, false, false, false, false, false};
    for (int i = 0; i < 6; i++)
    {
        if (World::GetBlock(positions[i]).IsSolid)
        {
            blockBoundingBox = Collision::BoundingBoxCentered(positions[i], 1.0f, 1.0f, 1.0f);
            tests[i] = Collision::IsIntersect(m_BoundingBox, blockBoundingBox);
        }
    }
    if (tests[0] || tests[1] || tests[2] || tests[3] || tests[4] || tests[5])
    {
        m_Position.z = m_LastPosition.z;
        m_Velocity.z = 0.0f;
        UpdateBoundingBox();
    }
}
