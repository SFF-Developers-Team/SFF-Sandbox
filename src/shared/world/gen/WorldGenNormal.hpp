#pragma once
#include <world/gen/WorldGen.hpp>
#include <PerlinNoise.hpp>

class WorldGenNormal : public WorldGen {
private:
    siv::PerlinNoise m_perlinNoise;

public:
    WorldGenNormal(std::shared_ptr<World> world, uint64_t seed);
    virtual std::shared_ptr<Chunk> generateChunk(int32_t position) override;
};