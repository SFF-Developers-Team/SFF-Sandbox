#pragma once

#include <world/Block.hpp>
#include <memory>

class World;
class Chunk;

class WorldGen {
protected:
    int64_t m_seed = 0;
    WorldGenType m_type;

public:
    WorldGen(uint64_t seed);

    virtual std::shared_ptr<Chunk> generateChunk(Vec2i position);
    void generateBlock(std::shared_ptr<Chunk> chunk, BlockPosition position, ItemID id);

    WorldGenType getType() { return m_type; }
    int64_t getSeed() { return m_seed; }
};