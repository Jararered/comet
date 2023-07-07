#include "Player.h"

#include <Entities/Entity.h>
#include <Input/Input.h>
#include <physics/Constants.h>

#include <cmath>

using namespace Comet;

Player::Player(World* world)
    : m_World(world)
{
    SetPosition({ 0.0f, 50.0f, 0.0f });
    Camera::SetPosition(m_Position);
}

Player::~Player()
{
}

void Player::Update()
{
    GetRequestedChunks();
}

void Player::FrameUpdate(float dt)
{
    Entity::FrameUpdate(dt);

    if (Input::IsLeftClick() && !m_BreakingBlock)
        Player::BreakBlock();
    if (!Input::IsLeftClick())
        m_BreakingBlock = false;

    if (Input::IsRightClick() && !m_PlacingBlock)
        Player::PlaceBlock();
    if (!Input::IsRightClick())
        m_PlacingBlock = false;

    // Update movement
    ProcessMovement(dt);

    // Update camera movement
    ProcessRotation();
    UpdateCamera();
}
void Player::GetRequestedChunks()
{
    glm::ivec3 newChunkIndex = m_World->GetChunkIndex(m_Position);

    m_World->ProcessRequestedChunks(newChunkIndex);
}

void Player::PlaceBlock()
{
    m_PlacingBlock = true;
    float step = 1.0f / 16.0f;
    glm::vec3 direction = m_Direction;
    glm::vec3 position = m_Position;
    glm::vec3 positionLast = m_Position;

    bool first = true;

    while (glm::length(direction) < 5.0f)
    {
        direction += glm::normalize(direction) * step;
        if (m_World->GetBlock(round(position + direction)).ID != 0)
        {
            // If there is a block on the first check, player is either
            // inside of a block or way too close.
            if (first)
            {
                return;
            }

            m_World->SetBlock(round(positionLast), Blocks::GetBlockFromID(m_SelectedBlock));
            return;
        }
        positionLast = position + direction;
        first = false;
    }
}

void Player::BreakBlock()
{
    m_BreakingBlock = true;
    float step = 1.0f / 16.0f;
    glm::vec3 direction = m_Direction;
    glm::vec3 position = m_Position;

    while (glm::length(direction) < 5.0f)
    {
        direction += glm::normalize(direction) * step;
        if (m_World->GetBlock(round(position + direction)).ID != 0)
        {
            m_World->SetBlock(round(position + direction), Blocks::Air());
            return;
        }
    }
}

void Player::ProcessMovement(float dt)
{
    // Checks for first frame or if a lag spike occurs
    if (dt < 0.0001f || dt > 0.25f)
    {
        return;
    }

    if (m_Flying)
    {
        // Basic movement processing
        // Used so when walking forward vertical movement doesn't occur.
        float movementSpeed = m_MovementSpeed;
        glm::vec3 direction = { 0.0f, 0.0f, 0.0f };
        glm::vec3 cameraFowardXZ = { m_ForwardVector.x, 0.0f, m_ForwardVector.z };

        // Speed increase
        if (Input::IsKeyPressed(KEY_LEFT_CONTROL))
            movementSpeed *= 10;

        // WASD movement
        if (Input::IsKeyPressed(KEY_W))
            direction += cameraFowardXZ;
        if (Input::IsKeyPressed(KEY_S))
            direction -= cameraFowardXZ;
        if (Input::IsKeyPressed(KEY_A))
            direction -= m_RightVector;
        if (Input::IsKeyPressed(KEY_D))
            direction += m_RightVector;

        // Fixes diagonal directed movement to not be faster than along an axis.
        // Only happens when holding two buttons that are off axis from each other.
        if (direction.x != 0.0f || direction.y != 0.0f)
            direction = glm::normalize(direction);

        // Still perform up/down movements after normalization.
        // Don't care about limiting speed along verticals.
        if (Input::IsKeyPressed(KEY_LEFT_SHIFT))
            direction -= Camera::POSITIVE_Y;

        if (Input::IsKeyPressed(KEY_SPACE))
            direction += Camera::POSITIVE_Y;

        ApplyMovement(direction * movementSpeed * dt);
    }
}

void Player::ProcessRotation()
{
    glm::vec2 mouseMovement = Input::GetMouseMovement();
    m_Yaw += (mouseMovement.x * m_RotationSpeed) / 300.0;
    m_Pitch += (mouseMovement.y * m_RotationSpeed) / 300.0;

    // Keep yaw angle from getting to imprecise
    if (m_Yaw > glm::radians(360.0f))
        m_Yaw -= glm::radians(360.0f);
    if (m_Yaw < glm::radians(-360.0f))
        m_Yaw += glm::radians(360.0f);

    // Keep pitch angle from going too far over
    if (m_Pitch > glm::radians(89.0f))
        m_Pitch = glm::radians(89.0f);
    if (m_Pitch < glm::radians(-89.0f))
        m_Pitch = glm::radians(-89.0f);

    m_Direction.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
    m_Direction.y = glm::sin(m_Pitch);
    m_Direction.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);

    m_ForwardVector = glm::normalize(m_Direction);
    m_RightVector = glm::cross(m_ForwardVector, Camera::POSITIVE_Y);
}

void Player::UpdateCamera()
{
    Camera::SetPosition(m_Position);
    Camera::SetForwardVector(m_ForwardVector);
}

void Player::UpdateBoundingBox()
{
    m_BoundingBox = {
        m_Position.x + (0.5f * m_Width), m_Position.x - (0.5f * m_Width),
        m_Position.y + 0.25f, m_Position.y - m_Height,
        m_Position.z + (0.5f * m_Width), m_Position.z - (0.5f * m_Width)
    };
}