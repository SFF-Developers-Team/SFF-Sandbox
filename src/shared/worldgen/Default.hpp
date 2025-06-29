#pragma once
#include "WorldGenerator.hpp"

class Default : public WorldGenerator {
public:
    using WorldGenerator::WorldGenerator;
    
    virtual void GenerateChunk(Chunk& chunk, Vector2i pos);
};