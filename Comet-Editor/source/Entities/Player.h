#pragma once

#include <Entities/Entity.h>
#include <Entities/EntityManager.h>
#include <Physics/Collision.h>
#include <Renderer/Renderer.h>

#include "World/BlockLibrary.h"
#include "World/Chunk.h"
#include "World/World.h"

class Player : public Entity
{
public:
    Player() = default;
    Player(World* world);
    ~Player();

    void Update();
    void FrameUpdate(float dt);

    void PlaceBlock();
    void BreakBlock();

    void ProcessMovement(float dt);
    void ProcessRotation();
    void UpdateCamera();

    int GetRenderDistance() { return m_RenderDistance; }

    void ProcessCollision();
    void UpdateBoundingBox();

    glm::vec3 ViewPosition() const;

    void GetRequestedChunks();

    void SetRenderDistance(int distance) { m_RenderDistance = distance; }

private:
    World* m_World;

    // Player States
    bool m_InWater = false;
    bool m_Flying = false;
    bool m_Standing = false;
    bool m_BreakingBlock = false;
    bool m_PlacingBlock = false;
    bool m_ThirdPersonCamera = false;
    int m_RenderDistance = 16;

    glm::vec3 m_GravityVel = {0.0f, 0.0f, 0.0f};

    // Purposfully setting this to an invalid index so that an update happens when
    // spawning in chunk 0, 0, 0
    glm::ivec3 m_ChunkIndex = {0, 1, 0};
    unsigned char m_SelectedBlock = Blocks::Stone().ID;
    Block m_LastBlockInsideOf;

    glm::ivec3 m_LookingAtBlock;
    Geometry m_BlockOverlayGeometry;
    Shader m_BlockOverlayShader;
    Mesh m_BlockOverlayMesh;

    double oldOffset = 0.0;
    double newOffset = 0.0;

    bool m_FlyToggleKeyWasDown = false;
    bool m_JumpKeyWasDown = false;

    // Collision parameters
    Collision m_BoundingBox;
    float m_Height = 1.5f;
    float m_Width = 0.75f;
    float m_HeadClearance = 0.25f;
    float m_CrouchLowerAmount = 0.2f;
    float m_CollisionHeight = 1.5f;
    float m_CollisionHeadClearance = 0.25f;
    float m_JumpSpeed = 7.0f; // upward m/s impulse for Verlet (pos - lastPos) / dt

    float m_MovementSpeed = 4.317f; // m/s
    float m_RotationSpeed = 1.5f;
    glm::vec3 m_ForwardVector = {0.0f, 0.0f, -1.0f};
    glm::vec3 m_Direction = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_RightVector = {1.0f, 0.0f, 0.0f};
    float m_Yaw = glm::radians(0.0f);
    float m_Pitch = glm::radians(0.0f);

public:
    glm::ivec3 ChunkIndex() const { return m_ChunkIndex; }
    void SetChunkIndex(const glm::ivec3& ChunkIndex) { m_ChunkIndex = ChunkIndex; }

    unsigned char SelectedBlock() const { return m_SelectedBlock; }
    void SetSelectedBlock(unsigned char id) { m_SelectedBlock = id; }

    bool IsInWater() const { return m_InWater; }
    void SetInWater(bool InWater) { m_InWater = InWater; }
};
