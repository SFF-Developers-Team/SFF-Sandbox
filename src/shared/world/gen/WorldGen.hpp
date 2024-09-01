#pragma once

#include <Block.hpp>
#include <memory>
class World;
class Chunk;

class WorldGen {
public:
    enum Type : uint8_t {
        NORMAL
    };

protected:
    World* m_world;
    int64_t m_seed = 0;
    Type m_type;
    
public:
    WorldGen(World* world, uint64_t seed);

    virtual std::shared_ptr<Chunk> generateChunk(int32_t position) = 0;

    auto getType() { return m_type; }
    auto getSeed() { return m_seed; }
};