#pragma once

#include "Chunk.hpp"
#include <cstdint>
#include <PerlinNoise.hpp>
#include "Types.hpp"

class WorldGenerator {
public:
    WorldGenerator(uint64_t seed) : m_perlin(seed) {}

    virtual void GenerateChunk(Chunk& chunk, Vector2i pos) {}

protected:
    siv::PerlinNoise m_perlin;
};