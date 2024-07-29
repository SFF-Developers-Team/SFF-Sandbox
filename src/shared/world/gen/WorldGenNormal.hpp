#pragma once
#include <PerlinNoise.hpp>
#include <WorldGen.hpp>

class WorldGenNormal : public WorldGen {
private:
    siv::PerlinNoise m_perlinNoise;

public:
    WorldGenNormal(World* world);
    ~WorldGenNormal();

    virtual Block* generateBlock(int x, int y, uint8_t layer) override;
};