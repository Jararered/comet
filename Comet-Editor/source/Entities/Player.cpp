#include "Player.h"

#include <Entities/Entity.h>
#include <Input/Input.h>
#include <Physics/Constants.h>
#include <Physics/VoxelCollision.h>

#include <algorithm>
#include <cmath>

using namespace Comet;

Player::Player(World* world) : m_World(world)
{
    SetPosition({0.0f, 50.0f, 0.0f});
    m_CollisionHeight = m_Height;
    m_CollisionHeadClearance = m_HeadClearance;
    Camera::SetPosition(ViewPosition());
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
    const float frameDt = std::clamp(dt, 0.0001f, 0.25f);

    const bool flyKeyDown = Input::IsKeyPressed(KEY_F);
    if (flyKeyDown && !m_FlyToggleKeyWasDown)
        m_Flying = !m_Flying;
    m_FlyToggleKeyWasDown = flyKeyDown;

    m_Gravity = m_Flying ? glm::vec3{0.0f, 0.0f, 0.0f} : Constants::Gravity;

    const bool crouchHeld = !m_Flying && Input::IsKeyPressed(KEY_LEFT_SHIFT);
    m_CollisionHeight = m_Height - (crouchHeld ? m_CrouchLowerAmount : 0.0f);
    m_CollisionHeadClearance = std::max(0.05f, m_HeadClearance - (crouchHeld ? m_CrouchLowerAmount : 0.0f));

    if (Input::IsLeftClick() && !m_BreakingBlock)
        Player::BreakBlock();
    if (!Input::IsLeftClick())
        m_BreakingBlock = false;

    if (Input::IsRightClick() && !m_PlacingBlock)
        Player::PlaceBlock();
    if (!Input::IsRightClick())
        m_PlacingBlock = false;

    ProcessMovement(frameDt);

    Entity::FrameUpdate(frameDt);

    if (!m_Flying)
        ProcessCollision();

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
    glm::vec3 position = ViewPosition();
    glm::vec3 positionLast = ViewPosition();

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
    glm::vec3 position = ViewPosition();

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
    float movementSpeed = m_MovementSpeed;
    if (Input::IsKeyPressed(KEY_LEFT_CONTROL))
        movementSpeed *= m_Flying ? 10.0f : 2.0f;

    if (m_Flying)
    {
        glm::vec3 direction = {0.0f, 0.0f, 0.0f};
        glm::vec3 cameraForwardXZ = {m_ForwardVector.x, 0.0f, m_ForwardVector.z};
        if (glm::length(cameraForwardXZ) > 1e-5f)
            cameraForwardXZ = glm::normalize(cameraForwardXZ);

        if (Input::IsKeyPressed(KEY_W))
            direction += cameraForwardXZ;
        if (Input::IsKeyPressed(KEY_S))
            direction -= cameraForwardXZ;
        if (Input::IsKeyPressed(KEY_A))
            direction -= m_RightVector;
        if (Input::IsKeyPressed(KEY_D))
            direction += m_RightVector;

        if (direction.x != 0.0f || direction.z != 0.0f)
            direction = glm::normalize(direction);

        if (Input::IsKeyPressed(KEY_LEFT_SHIFT))
            direction -= Camera::POSITIVE_Y;
        if (Input::IsKeyPressed(KEY_SPACE))
            direction += Camera::POSITIVE_Y;

        ApplyMovement(direction * movementSpeed * dt);
        m_JumpKeyWasDown = Input::IsKeyPressed(KEY_SPACE);
    }
    else
    {
        glm::vec3 forwardXZ = m_ForwardVector;
        forwardXZ.y = 0.0f;
        if (glm::length(forwardXZ) > 1e-5f)
            forwardXZ = glm::normalize(forwardXZ);

        glm::vec3 rightXZ = glm::cross(forwardXZ, Camera::POSITIVE_Y);
        if (glm::length(rightXZ) > 1e-5f)
            rightXZ = glm::normalize(rightXZ);

        glm::vec3 direction = {0.0f, 0.0f, 0.0f};
        if (Input::IsKeyPressed(KEY_W))
            direction += forwardXZ;
        if (Input::IsKeyPressed(KEY_S))
            direction -= forwardXZ;
        if (Input::IsKeyPressed(KEY_A))
            direction -= rightXZ;
        if (Input::IsKeyPressed(KEY_D))
            direction += rightXZ;

        if (direction.x != 0.0f || direction.z != 0.0f)
            direction = glm::normalize(direction);

        ApplyMovement(direction * movementSpeed * dt);

        const bool jumpKey = Input::IsKeyPressed(KEY_SPACE);
        const bool grounded = Comet::Physics::IsGrounded(
            m_Position,
            m_Width,
            m_CollisionHeight,
            m_CollisionHeadClearance,
            [this](const glm::ivec3& cell) { return m_World->GetBlock(cell).IsSolid; });
        // Verlet: implicit velocity is (pos - last) / dt. Boost upward by jumpSpeed m/s this frame.
        if (grounded && jumpKey && !m_JumpKeyWasDown)
            m_LastPosition.y -= m_JumpSpeed * dt;
        m_JumpKeyWasDown = jumpKey;
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

glm::vec3 Player::ViewPosition() const
{
    const bool crouchHeld = !m_Flying && Input::IsKeyPressed(KEY_LEFT_SHIFT);
    if (!crouchHeld)
        return m_Position;
    return m_Position + glm::vec3(0.0f, -m_CrouchLowerAmount, 0.0f);
}

void Player::UpdateCamera()
{
    Camera::SetPosition(ViewPosition());
    Camera::SetForwardVector(m_ForwardVector);
}

void Player::UpdateBoundingBox()
{
    m_BoundingBox = Comet::Physics::PlayerBoundingBox(m_Position, m_Width, m_CollisionHeight, m_CollisionHeadClearance);
}

void Player::ProcessCollision()
{
    Comet::Physics::ResolveVoxelCollisions(
        m_Position,
        m_LastPosition,
        m_Width,
        m_CollisionHeight,
        m_CollisionHeadClearance,
        [this](const glm::ivec3& cell) { return m_World->GetBlock(cell).IsSolid; });

    UpdateBoundingBox();

    m_Standing = Comet::Physics::IsGrounded(
        m_Position,
        m_Width,
        m_CollisionHeight,
        m_CollisionHeadClearance,
        [this](const glm::ivec3& cell) { return m_World->GetBlock(cell).IsSolid; });
}