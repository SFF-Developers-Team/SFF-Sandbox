#pragma once

#include <WorldGen.hpp>

class WorldGenFlat : public WorldGen {
public:
    virtual Block::BlockType generateBlock(int x, int y) override;
};