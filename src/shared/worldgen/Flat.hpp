#pragma once
#include "WorldGenerator.hpp"

class Flat : public WorldGenerator {
public:
    using WorldGenerator::WorldGenerator;
    
    virtual void GenerateChunk(Chunk& chunk, Vector2i pos);
};