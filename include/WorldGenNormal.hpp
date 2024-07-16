#pragma once

#include <WorldGen.hpp>
#include <raylib.h>

class WorldGenNormal : public WorldGen {
private:
    Image m_perlinNoise;
public:
    WorldGenNormal(World* world);
    ~WorldGenNormal();

    virtual Block* generateBlock(int x, int y) override;
};