#pragma once

#include <comet.pch>

#include "Renderer.h"
#include "handlers/Entity.h"
#include "handlers/EntityHandler.h"
#include "handlers/MouseHandler.h"

#include "world/Chunk.h"
#include "world/World.h"
#include "world/BlockLibrary.h"

class Player : public Entity
{
  public:
    Player();
    ~Player();

    void Update() override;
    void FrameUpdate() override;

    void PlaceBlock();
    void BreakBlock();
    void ProcessClicks();
    void ProcessKeys();
    void ProcessScrolls();

    void GetRequestedChunks();

  private:
    // Purposfully setting this to an invalid index so that an update happens when
    // spawning in chunk 0, 0, 0
    glm::ivec3 m_ChunkIndex = {0, 1, 0};
    unsigned char m_SelectedBlock = Blocks::Torch().ID;
    Block m_LastBlockInsideOf;
    bool m_InWater = false;

    double oldOffset = 0.0;
    double newOffset = 0.0;

  public:
    glm::ivec3 ChunkIndex() const { return m_ChunkIndex; }
    void SetChunkIndex(const glm::ivec3 &ChunkIndex) { m_ChunkIndex = ChunkIndex; }

    unsigned char SelectedBlock() const { return m_SelectedBlock; }
    void SetSelectedBlock(unsigned char id) { m_SelectedBlock = id; }

    bool IsInWater() const { return m_InWater; }
    void SetInWater(bool InWater) { m_InWater = InWater; }


};
