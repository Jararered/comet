#pragma once

struct Block
{
    unsigned char ID;
    bool IsTransparent;
    bool IsSolid;
    bool IsCrossGeometry;

    template <class Archive> void serialize(Archive &ar) { ar(ID); }
};
