#pragma once

#include "Chunk.hpp"
#include <cstdint>

class WorldGenerator {
public:
    WorldGenerator(uint64_t seed) : m_seed(seed) {}

    virtual void GenerateChunk(Chunk& chunk, Vector2i pos) {}

private:
    uint64_t m_seed;
};