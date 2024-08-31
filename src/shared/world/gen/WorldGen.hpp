#pragma once

#include <Block.hpp>
#include <memory>
class World;
class Chunk;

class WorldGen {
protected:
    World* m_world;
    uint64_t m_seed = 0;
    
public:
    WorldGen(World* world, uint64_t seed);

    virtual std::shared_ptr<Chunk> generateChunk(int32_t position) = 0;
};