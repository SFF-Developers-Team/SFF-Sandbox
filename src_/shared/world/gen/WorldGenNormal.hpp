#pragma once
#include <world/gen/WorldGen.hpp>
#include <PerlinNoise.hpp>
#include <random>

class WorldGenNormal : public WorldGen {
private:
    siv::PerlinNoise m_perlinNoise;
    std::mt19937 m_random;

public:
    WorldGenNormal(std::shared_ptr<World> world, uint64_t seed);
    virtual std::shared_ptr<Chunk> generateChunk(int32_t position) override;
};