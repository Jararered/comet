#pragma once

#include "Block.h"

#include <array>

// Enum for convenience
enum ID
{
    Air,                       // 0
    Stone,                     // 1
    Grass,                     // 2
    Dirt,                      // 3
    Cobblestone,               // 4
    Oak_Planks,                // 5
    Oak_Sapling,               // 6
    Bedrock,                   // 7
    Water,                     // 8
    Water2,                    // 9
    Lava,                      // 10
    Lava2,                     // 11
    Sand,                      // 12
    Gravel,                    // 13
    Gold_Ore,                  // 14
    Iron_Ore,                  // 15
    Coal_Ore,                  // 16
    Oak_Log,                   // 17
    Oak_Leaves,                // 18
    Sponge,                    // 19
    Glass,                     // 20
    Lapis_Lazuli_Ore,          // 21
    Lapis_Lazuli_Block,        // 22
    Dispenser,                 // 23
    Sandstone,                 // 24
    Noteblock,                 // 25
    _Bed,                      // 26
    Powered_Rail,              // 27
    Detector_Rail,             // 28
    Stickey_Piston,            // 29
    Cobweb,                    // 30
    _Dead_Oak_Sapling,         // 31
    _Dead_Sapling,             // 32
    Piston,                    // 33
    _Piston_Head,              // 34
    White_Wool,                // 35
    _Grass_Top,                // 36
    Yellow_Flower,             // 37
    Red_Flower,                // 38
    Brown_Mushroom,            // 39
    Red_Mushroom,              // 40
    Gold_Block,                // 41
    Iron_Block,                // 42
    Smoothstone_Stacked_Slabs, // 43
    Smoothstone_Slab,          // 44
    Bricks,                    // 45
    TNT,                       // 46
    Bookshelf,                 // 47
    Mossy_Cobblestone,         // 48
    Obsidian,                  // 49
    Torch,                     // 50
    Fire,                      // 51
    Monster_Spawner,           // 52
    Oak_Stairs,                // 53
    Chest,                     // 54
    _Redstone,                 // 55
    Diamond_Ore,               // 56
    Diamond_Block,             // 57
    Crafting_Table,            // 58
};

class Blocks
{
public:
    inline static auto& Instance()
    {
        static Blocks instance;
        return instance;
    }

    // ID, Transparent, Solid, Cross-Shape
    inline static Block Air() { return Block(ID::Air, true, false, Block::Shapes::Cube); }
    inline static Block Stone() { return Block(ID::Stone, false, true, Block::Shapes::Cube); }
    inline static Block Grass() { return Block(ID::Grass, false, true, Block::Shapes::Cube); }
    inline static Block Dirt() { return Block(ID::Dirt, false, true, Block::Shapes::Cube); }
    inline static Block Cobblestone() { return Block(ID::Cobblestone, false, true, Block::Shapes::Cube); }
    inline static Block Oak_Planks() { return Block(ID::Oak_Planks, false, true, Block::Shapes::Cube); }
    inline static Block Oak_Sapling() { return Block(ID::Oak_Sapling, true, false, Block::Shapes::Cube); }
    inline static Block Bedrock() { return Block(ID::Bedrock, false, true, Block::Shapes::Cube); }
    inline static Block Water() { return Block(ID::Water, true, false, Block::Shapes::Cube); }
    inline static Block Water2() { return Block(ID::Water2, false, true, Block::Shapes::Cube); }
    inline static Block Lava() { return Block(ID::Lava, false, true, Block::Shapes::Cube); }
    inline static Block Lava2() { return Block(ID::Lava2, false, true, Block::Shapes::Cube); }
    inline static Block Sand() { return Block(ID::Sand, false, true, Block::Shapes::Cube); }
    inline static Block Gravel() { return Block(ID::Gravel, false, true, Block::Shapes::Cube); }
    inline static Block Gold_Ore() { return Block(ID::Gold_Ore, false, true, Block::Shapes::Cube); }
    inline static Block Iron_Ore() { return Block(ID::Iron_Ore, false, true, Block::Shapes::Cube); }
    inline static Block Coal_Ore() { return Block(ID::Coal_Ore, false, true, Block::Shapes::Cube); }
    inline static Block Oak_Log() { return Block(ID::Oak_Log, false, true, Block::Shapes::Cube); }
    inline static Block Oak_Leaves() { return Block(ID::Oak_Leaves, true, true, Block::Shapes::Cube); }
    inline static Block Sponge() { return Block(ID::Sponge, false, true, Block::Shapes::Cube); }
    inline static Block Glass() { return Block(ID::Glass, true, true, Block::Shapes::Cube); }
    inline static Block Lapis_Lazuli_Ore() { return Block(ID::Lapis_Lazuli_Ore, false, true, Block::Shapes::Cube); }
    inline static Block Lapis_Lazuli_Block() { return Block(ID::Lapis_Lazuli_Block, false, true, Block::Shapes::Cube); }
    inline static Block Dispenser() { return Block(ID::Dispenser, false, true, Block::Shapes::Cube); }
    inline static Block Sandstone() { return Block(ID::Sandstone, false, true, Block::Shapes::Cube); }
    inline static Block Noteblock() { return Block(ID::Noteblock, false, true, Block::Shapes::Cube); }
    inline static Block _Bed() { return Block(ID::_Bed, false, true, Block::Shapes::Cube); }
    inline static Block Powered_Rail() { return Block(ID::Powered_Rail, true, false, Block::Shapes::Cube); }
    inline static Block Detector_Rail() { return Block(ID::Detector_Rail, true, false, Block::Shapes::Cube); }
    inline static Block Stickey_Piston() { return Block(ID::Stickey_Piston, false, true, Block::Shapes::Cube); }
    inline static Block Cobweb() { return Block(ID::Cobweb, true, false, Block::Shapes::Cross); }
    inline static Block _Dead_Oak_Sapling() { return Block(ID::_Dead_Oak_Sapling, false, true, Block::Shapes::Cross); }
    inline static Block _Dead_Sapling() { return Block(ID::_Dead_Sapling, false, true, Block::Shapes::Cross); }
    inline static Block Piston() { return Block(ID::Piston, false, true, Block::Shapes::Cube); }
    inline static Block _Piston_Head() { return Block(ID::_Piston_Head, false, true, Block::Shapes::Cube); }
    inline static Block White_Wool() { return Block(ID::White_Wool, false, true, Block::Shapes::Cube); }
    inline static Block _Grass_Top() { return Block(ID::_Grass_Top, false, true, Block::Shapes::Cube); }
    inline static Block Yellow_Flower() { return Block(ID::Yellow_Flower, true, false, Block::Shapes::Cross); }
    inline static Block Red_Flower() { return Block(ID::Red_Flower, true, false, Block::Shapes::Cross); }
    inline static Block Brown_Mushroom() { return Block(ID::Brown_Mushroom, true, false, Block::Shapes::Cross); }
    inline static Block Red_Mushroom() { return Block(ID::Red_Mushroom, true, false, Block::Shapes::Cross); }
    inline static Block Gold_Block() { return Block(ID::Gold_Block, false, true, Block::Shapes::Cube); }
    inline static Block Iron_Block() { return Block(ID::Iron_Block, false, true, Block::Shapes::Cube); }
    inline static Block Smoothstone_Stacked_Slabs() { return Block(ID::Smoothstone_Stacked_Slabs, false, true, Block::Shapes::Half); }
    inline static Block Smoothstone_Slab() { return Block(ID::Smoothstone_Slab, true, true, Block::Shapes::Half); }
    inline static Block Bricks() { return Block(ID::Bricks, false, true, Block::Shapes::Cube); }
    inline static Block TNT() { return Block(ID::TNT, false, true, Block::Shapes::Cube); }
    inline static Block Bookshelf() { return Block(ID::Bookshelf, false, true, Block::Shapes::Cube); }
    inline static Block Mossy_Cobblestone() { return Block(ID::Mossy_Cobblestone, false, true, Block::Shapes::Cube); }
    inline static Block Obsidian() { return Block(ID::Obsidian, false, true, Block::Shapes::Cube); }
    inline static Block Torch() { return Block(ID::Torch, true, false, Block::Shapes::Torch); }
    inline static Block Fire() { return Block(ID::Fire, true, false, Block::Shapes::Cube); }
    inline static Block Monster_Spawner() { return Block(ID::Monster_Spawner, true, true, Block::Shapes::Cube); }
    inline static Block Oak_Stairs() { return Block(ID::Oak_Stairs, false, true, Block::Shapes::Cube); }
    inline static Block Chest() { return Block(ID::Chest, false, true, Block::Shapes::Cube); }
    inline static Block _Redstone() { return Block(ID::_Redstone, false, true, Block::Shapes::Cube); }
    inline static Block Diamond_Ore() { return Block(ID::Diamond_Ore, false, true, Block::Shapes::Cube); }
    inline static Block Diamond_Block() { return Block(ID::Diamond_Block, false, true, Block::Shapes::Cube); }
    inline static Block Crafting_Table() { return Block(ID::Crafting_Table, false, true, Block::Shapes::Cube); }

    static void Initialize()
    {
        // Assigning block IDs
        Instance().m_BlockIndices.at(ID::Air) = {0, 0, 0, 0, 0, 0};
        Instance().m_BlockIndices.at(ID::Stone) = {1, 1, 1, 1, 1, 1};
        Instance().m_BlockIndices.at(ID::Grass) = {3, 3, 0, 2, 3, 3};
        Instance().m_BlockIndices.at(ID::Dirt) = {2, 2, 2, 2, 2, 2};
        Instance().m_BlockIndices.at(ID::Cobblestone) = {16, 16, 16, 16, 16, 16};
        Instance().m_BlockIndices.at(ID::Oak_Planks) = {4, 4, 4, 4, 4, 4};
        Instance().m_BlockIndices.at(ID::Oak_Sapling) = {15, 15, 15, 15, 15, 15};
        Instance().m_BlockIndices.at(ID::Bedrock) = {17, 17, 17, 17, 17, 17};
        Instance().m_BlockIndices.at(ID::Water) = {207, 207, 207, 207, 207, 207};
        Instance().m_BlockIndices.at(ID::Water2) = {207, 207, 207, 207, 207, 207};
        Instance().m_BlockIndices.at(ID::Lava) = {255, 255, 255, 255, 255, 255};
        Instance().m_BlockIndices.at(ID::Lava2) = {255, 255, 255, 255, 255, 255};
        Instance().m_BlockIndices.at(ID::Sand) = {18, 18, 18, 18, 18, 18};
        Instance().m_BlockIndices.at(ID::Gravel) = {19, 19, 19, 19, 19, 19};
        Instance().m_BlockIndices.at(ID::Gold_Ore) = {32, 32, 32, 32, 32, 32};
        Instance().m_BlockIndices.at(ID::Iron_Ore) = {33, 33, 33, 33, 33, 33};
        Instance().m_BlockIndices.at(ID::Coal_Ore) = {34, 34, 34, 34, 34, 34};
        Instance().m_BlockIndices.at(ID::Oak_Log) = {20, 20, 21, 21, 20, 20};
        Instance().m_BlockIndices.at(ID::Oak_Leaves) = {52, 52, 52, 52, 52, 52};
        Instance().m_BlockIndices.at(ID::Sponge) = {48, 48, 48, 48, 48, 48};
        Instance().m_BlockIndices.at(ID::Glass) = {49, 49, 49, 49, 49, 49};
        Instance().m_BlockIndices.at(ID::Lapis_Lazuli_Ore) = {160, 160, 160, 160, 160, 160};
        Instance().m_BlockIndices.at(ID::Lapis_Lazuli_Block) = {144, 144, 144, 144, 144, 144};
        Instance().m_BlockIndices.at(ID::Dispenser) = {46, 45, 1, 1, 45, 45};
        Instance().m_BlockIndices.at(ID::Sandstone) = {192, 192, 176, 208, 192, 192};
        Instance().m_BlockIndices.at(ID::Noteblock) = {74, 74, 75, 74, 74, 74};
        Instance().m_BlockIndices.at(ID::_Bed) = {58, 58, 59, 58, 58, 58};
        Instance().m_BlockIndices.at(ID::Powered_Rail) = {163, 163, 163, 163, 163, 163};
        Instance().m_BlockIndices.at(ID::Detector_Rail) = {195, 195, 195, 195, 195, 195};
        Instance().m_BlockIndices.at(ID::Stickey_Piston) = {84, 84, 82, 85, 84, 84}; // Wrong
        Instance().m_BlockIndices.at(ID::Cobweb) = {11, 11, 11, 11, 11, 11};
        Instance().m_BlockIndices.at(ID::_Dead_Oak_Sapling) = {43, 43, 43, 43, 43, 43}; // Wrong
        Instance().m_BlockIndices.at(ID::_Dead_Sapling) = {43, 43, 43, 43, 43, 43};     // Wrong
        Instance().m_BlockIndices.at(ID::Piston) = {84, 84, 83, 85, 84, 84};            // Wrong
        Instance().m_BlockIndices.at(ID::_Piston_Head) = {83, 83, 83, 83, 83, 83};      // Wrong
        Instance().m_BlockIndices.at(ID::White_Wool) = {64, 64, 64, 64, 64, 64};
        Instance().m_BlockIndices.at(ID::_Grass_Top) = {0, 0, 0, 0, 0, 0};
        Instance().m_BlockIndices.at(ID::Yellow_Flower) = {13, 13, 13, 13, 13, 13};
        Instance().m_BlockIndices.at(ID::Red_Flower) = {12, 12, 12, 12, 12, 12};
        Instance().m_BlockIndices.at(ID::Brown_Mushroom) = {29, 29, 29, 29, 29, 29};
        Instance().m_BlockIndices.at(ID::Red_Mushroom) = {28, 28, 28, 28, 28, 28};
        Instance().m_BlockIndices.at(ID::Gold_Block) = {23, 23, 23, 23, 23, 23};
        Instance().m_BlockIndices.at(ID::Iron_Block) = {22, 22, 22, 22, 22, 22};
        Instance().m_BlockIndices.at(ID::Smoothstone_Stacked_Slabs) = {5, 5, 6, 6, 5, 5};
        Instance().m_BlockIndices.at(ID::Smoothstone_Slab) = {5, 5, 6, 6, 5, 5};
        Instance().m_BlockIndices.at(ID::Bricks) = {7, 7, 7, 7, 7, 7};
        Instance().m_BlockIndices.at(ID::TNT) = {8, 8, 9, 10, 8, 8};
        Instance().m_BlockIndices.at(ID::Bookshelf) = {35, 35, 4, 4, 35, 35};
        Instance().m_BlockIndices.at(ID::Mossy_Cobblestone) = {36, 36, 36, 36, 36, 36};
        Instance().m_BlockIndices.at(ID::Obsidian) = {37, 37, 37, 37, 37, 37};
        Instance().m_BlockIndices.at(ID::Torch) = {80, 80, 80, 80, 80, 80};
        Instance().m_BlockIndices.at(ID::Fire) = {32, 32, 32, 32, 32, 32};
        Instance().m_BlockIndices.at(ID::Monster_Spawner) = {65, 65, 65, 65, 65, 65};
        Instance().m_BlockIndices.at(ID::Oak_Stairs) = {4, 4, 4, 4, 4, 4};
        Instance().m_BlockIndices.at(ID::Chest) = {27, 26, 25, 25, 26, 26};
        Instance().m_BlockIndices.at(ID::_Redstone) = {164, 164, 164, 164, 164, 164};
        Instance().m_BlockIndices.at(ID::Diamond_Ore) = {50, 50, 50, 50, 50, 50};
        Instance().m_BlockIndices.at(ID::Diamond_Block) = {24, 24, 24, 24, 24, 24};
        Instance().m_BlockIndices.at(ID::Crafting_Table) = {60, 60, 43, 4, 59, 59};
    }

    static Block GetBlockFromID(unsigned char id)
    {
        switch (id)
        {
            case (ID::Air):
                return Air();
            case (ID::Stone):
                return Stone();
            case (ID::Grass):
                return Grass();
            case (ID::Dirt):
                return Dirt();
            case (ID::Cobblestone):
                return Cobblestone();
            case (ID::Oak_Planks):
                return Oak_Planks();
            case (ID::Oak_Sapling):
                return Oak_Sapling();
            case (ID::Bedrock):
                return Bedrock();
            case (ID::Water):
                return Water();
            case (ID::Water2):
                return Water2();
            case (ID::Lava):
                return Lava();
            case (ID::Lava2):
                return Lava2();
            case (ID::Sand):
                return Sand();
            case (ID::Gravel):
                return Gravel();
            case (ID::Gold_Ore):
                return Gold_Ore();
            case (ID::Iron_Ore):
                return Iron_Ore();
            case (ID::Coal_Ore):
                return Coal_Ore();
            case (ID::Oak_Log):
                return Oak_Log();
            case (ID::Oak_Leaves):
                return Oak_Leaves();
            case (ID::Sponge):
                return Sponge();
            case (ID::Glass):
                return Glass();
            case (ID::Lapis_Lazuli_Ore):
                return Lapis_Lazuli_Ore();
            case (ID::Lapis_Lazuli_Block):
                return Lapis_Lazuli_Block();
            case (ID::Dispenser):
                return Dispenser();
            case (ID::Sandstone):
                return Sandstone();
            case (ID::Noteblock):
                return Noteblock();
            case (ID::_Bed):
                return _Bed();
            case (ID::Powered_Rail):
                return Powered_Rail();
            case (ID::Detector_Rail):
                return Detector_Rail();
            case (ID::Stickey_Piston):
                return Stickey_Piston();
            case (ID::Cobweb):
                return Cobweb();
            case (ID::_Dead_Oak_Sapling):
                return _Dead_Oak_Sapling();
            case (ID::_Dead_Sapling):
                return _Dead_Sapling();
            case (ID::Piston):
                return Piston();
            case (ID::_Piston_Head):
                return _Piston_Head();
            case (ID::White_Wool):
                return White_Wool();
            case (ID::_Grass_Top):
                return _Grass_Top();
            case (ID::Yellow_Flower):
                return Yellow_Flower();
            case (ID::Red_Flower):
                return Red_Flower();
            case (ID::Brown_Mushroom):
                return Brown_Mushroom();
            case (ID::Red_Mushroom):
                return Red_Mushroom();
            case (ID::Gold_Block):
                return Gold_Block();
            case (ID::Iron_Block):
                return Iron_Block();
            case (ID::Smoothstone_Stacked_Slabs):
                return Smoothstone_Stacked_Slabs();
            case (ID::Smoothstone_Slab):
                return Smoothstone_Slab();
            case (ID::Bricks):
                return Bricks();
            case (ID::TNT):
                return TNT();
            case (ID::Bookshelf):
                return Bookshelf();
            case (ID::Mossy_Cobblestone):
                return Mossy_Cobblestone();
            case (ID::Obsidian):
                return Obsidian();
            case (ID::Torch):
                return Torch();
            case (ID::Fire):
                return Fire();
            case (ID::Monster_Spawner):
                return Monster_Spawner();
            case (ID::Oak_Stairs):
                return Oak_Stairs();
            case (ID::Chest):
                return Chest();
            case (ID::_Redstone):
                return _Redstone();
            case (ID::Diamond_Ore):
                return Diamond_Ore();
            case (ID::Diamond_Block):
                return Diamond_Block();
            case (ID::Crafting_Table):
                return Crafting_Table();
        }
        return Air();
    }

    static std::array<unsigned char, 6> GetTextures(unsigned char blockID) { return Instance().m_BlockIndices.at(blockID); }

private:
    Blocks() {};
    Blocks(Blocks const&);
    void operator=(Blocks const&);

    std::array<std::array<unsigned char, 6>, 256> m_BlockIndices;
};
