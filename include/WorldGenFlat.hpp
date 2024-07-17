#pragma once

#include <WorldGen.hpp>

class WorldGenFlat : public WorldGen {
public:
    WorldGenFlat(World* world);

    virtual Block* generateBlock(int x, int y, uint8_t layer) override;
};