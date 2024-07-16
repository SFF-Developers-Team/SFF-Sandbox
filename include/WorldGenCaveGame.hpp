#pragma once

#include <WorldGen.hpp>
#include <PerlinNoise.hpp>

class WorldGenCaveGame : public WorldGen {
private:
    siv::PerlinNoise m_perlin;

public:
    WorldGenCaveGame(World* world);

    virtual Block* generateBlock(int x, int y) override;
};