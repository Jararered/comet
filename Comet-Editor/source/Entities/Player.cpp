#include "Player.h"

#include <Entities/Entity.h>
#include <Input/Input.h>
#include <Physics/Constants.h>
#include <Physics/VoxelCollision.h>

#include <algorithm>
#include <cmath>

using namespace Comet;

Player::Player(World* world, Comet::ViewCamera& camera) : m_World(world), m_Camera(&camera)
{
    SetPosition({0.0f, 50.0f, 0.0f});
    m_CollisionHeight = m_Height;
    m_CollisionHeadClearance = m_HeadClearance;
    m_Camera->SetPosition(ViewPosition());
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
    const bool flyKeyDown = Input::IsKeyPressed(KEY_F);
    const bool toggleFlying = flyKeyDown && !m_FlyToggleKeyWasDown;
    const float wheelMove = Input::GetMouseWheelMove();
    MovementInput movementInput;
    movementInput.Forward = Input::IsKeyPressed(KEY_W);
    movementInput.Backward = Input::IsKeyPressed(KEY_S);
    movementInput.Left = Input::IsKeyPressed(KEY_A);
    movementInput.Right = Input::IsKeyPressed(KEY_D);
    movementInput.Jump = Input::IsKeyPressed(KEY_SPACE);
    movementInput.Crouch = Input::IsKeyPressed(KEY_LEFT_SHIFT);
    movementInput.Sprint = Input::IsKeyPressed(KEY_LEFT_CONTROL);

    if (Input::IsLeftClick() && !m_BreakingBlock)
        Player::BreakBlock();
    if (!Input::IsLeftClick())
        m_BreakingBlock = false;

    if (Input::IsRightClick() && !m_PlacingBlock)
        Player::PlaceBlock();
    if (!Input::IsRightClick())
        m_PlacingBlock = false;

    {
        std::lock_guard lock(m_StateMutex);

        if (toggleFlying)
        {
            m_Flying = !m_Flying;
            m_LastPosition = m_Position;
            m_Acceleration = {0.0f, 0.0f, 0.0f};
        }
        m_FlyToggleKeyWasDown = flyKeyDown;

        if (m_Flying && wheelMove != 0.0f)
            m_FlySpeedMultiplier = std::clamp(m_FlySpeedMultiplier + wheelMove * 0.25f, 0.25f, 20.0f);

        m_MovementInput = movementInput;
        m_Crouching = !m_Flying && movementInput.Crouch;
        m_CollisionHeight = m_Height - (m_Crouching ? m_CrouchLowerAmount : 0.0f);
        m_CollisionHeadClearance = std::max(0.05f, m_HeadClearance - (m_Crouching ? m_CrouchLowerAmount : 0.0f));

        ProcessRotation();
        UpdateCamera();
    }
}

void Player::PhysicsUpdate(float dt)
{
    const float physicsDt = std::clamp(dt, 0.0001f, 0.05f);

    std::lock_guard lock(m_StateMutex);

    m_Gravity = m_Flying ? glm::vec3{0.0f, 0.0f, 0.0f} : Constants::Gravity;

    ProcessMovement(physicsDt);

    Entity::PhysicsUpdate(physicsDt);

    if (!m_Flying)
        ProcessCollision();
}
void Player::GetRequestedChunks()
{
    glm::vec3 position;
    {
        std::lock_guard lock(m_StateMutex);
        position = m_Position;
    }

    glm::ivec3 newChunkIndex = m_World->GetChunkIndex(position);
    SetChunkIndex(newChunkIndex);

    m_World->ProcessRequestedChunks(newChunkIndex);
}

void Player::PlaceBlock()
{
    m_PlacingBlock = true;
    float step = 1.0f / 16.0f;
    glm::vec3 direction;
    glm::vec3 position;
    {
        std::lock_guard lock(m_StateMutex);
        direction = m_Direction;
        position = ViewPosition();
    }
    glm::vec3 positionLast = position;

    bool first = true;

    while (glm::length(direction) < 5.0f)
    {
        direction += glm::normalize(direction) * step;
        if (m_World->GetBlock(round(position + direction)).ID != 0)
        {
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
    glm::vec3 direction;
    glm::vec3 position;
    {
        std::lock_guard lock(m_StateMutex);
        direction = m_Direction;
        position = ViewPosition();
    }

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
    if (m_MovementInput.Sprint)
        movementSpeed *= m_Flying ? 10.0f : 2.0f;

    if (m_Flying)
    {
        movementSpeed *= m_FlySpeedMultiplier;

        glm::vec3 direction = {0.0f, 0.0f, 0.0f};
        glm::vec3 cameraForwardXZ = {m_ForwardVector.x, 0.0f, m_ForwardVector.z};
        if (glm::length(cameraForwardXZ) > 1e-5f)
            cameraForwardXZ = glm::normalize(cameraForwardXZ);

        if (m_MovementInput.Forward)
            direction += cameraForwardXZ;
        if (m_MovementInput.Backward)
            direction -= cameraForwardXZ;
        if (m_MovementInput.Left)
            direction -= m_RightVector;
        if (m_MovementInput.Right)
            direction += m_RightVector;

        if (direction.x != 0.0f || direction.z != 0.0f)
            direction = glm::normalize(direction);

        if (m_MovementInput.Crouch)
            direction -= ViewCamera::POSITIVE_Y;
        if (m_MovementInput.Jump)
            direction += ViewCamera::POSITIVE_Y;

        ApplyMovement(direction * movementSpeed * dt);
        m_JumpKeyWasDown = m_MovementInput.Jump;
    }
    else
    {
        glm::vec3 forwardXZ = m_ForwardVector;
        forwardXZ.y = 0.0f;
        if (glm::length(forwardXZ) > 1e-5f)
            forwardXZ = glm::normalize(forwardXZ);

        glm::vec3 rightXZ = glm::cross(forwardXZ, ViewCamera::POSITIVE_Y);
        if (glm::length(rightXZ) > 1e-5f)
            rightXZ = glm::normalize(rightXZ);

        glm::vec3 direction = {0.0f, 0.0f, 0.0f};
        if (m_MovementInput.Forward)
            direction += forwardXZ;
        if (m_MovementInput.Backward)
            direction -= forwardXZ;
        if (m_MovementInput.Left)
            direction -= rightXZ;
        if (m_MovementInput.Right)
            direction += rightXZ;

        if (direction.x != 0.0f || direction.z != 0.0f)
            direction = glm::normalize(direction);

        ApplyMovement(direction * movementSpeed * dt);

        const bool jumpKey = m_MovementInput.Jump;
        const bool grounded = Comet::Physics::IsGrounded(
            m_Position,
            m_Width,
            m_CollisionHeight,
            m_CollisionHeadClearance,
            [this](const glm::ivec3& cell) { return m_World->GetBlock(cell).IsSolid; });
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

    if (m_Yaw > glm::radians(360.0f))
        m_Yaw -= glm::radians(360.0f);
    if (m_Yaw < glm::radians(-360.0f))
        m_Yaw += glm::radians(360.0f);

    if (m_Pitch > glm::radians(89.0f))
        m_Pitch = glm::radians(89.0f);
    if (m_Pitch < glm::radians(-89.0f))
        m_Pitch = glm::radians(-89.0f);

    m_Direction.x = glm::cos(m_Yaw) * glm::cos(m_Pitch);
    m_Direction.y = glm::sin(m_Pitch);
    m_Direction.z = glm::sin(m_Yaw) * glm::cos(m_Pitch);

    m_ForwardVector = glm::normalize(m_Direction);
    m_RightVector = glm::normalize(glm::cross(m_ForwardVector, ViewCamera::POSITIVE_Y));
}

glm::vec3 Player::ViewPosition() const
{
    std::lock_guard lock(m_StateMutex);

    if (!m_Crouching)
        return m_Position;
    return m_Position + glm::vec3(0.0f, -m_CrouchLowerAmount, 0.0f);
}

void Player::UpdateCamera()
{
    m_Camera->SetPosition(ViewPosition());
    m_Camera->SetForwardVector(m_ForwardVector);
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
