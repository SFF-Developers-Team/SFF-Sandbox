#pragma once

#include <WorldGen.hpp>

class WorldGenNormal : public WorldGen {
public:
    WorldGenNormal(World *world);

    virtual Block *generateBlock(int x, int y) override;
};