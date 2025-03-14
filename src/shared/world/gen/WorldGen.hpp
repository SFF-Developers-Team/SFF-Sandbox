#pragma once

#include <world/Block.hpp>
#include <memory>

class World;
class Chunk;

class WorldGen {
protected:
    std::shared_ptr<World> m_world;
    int64_t m_seed = 0;
    WorldGenType m_type;

public:
    WorldGen(std::shared_ptr<World> world, uint64_t seed);

    virtual std::shared_ptr<Chunk> generateChunk(Vec2i position) = 0;
    void generateBlock(std::shared_ptr<Chunk> chunk, BlockPosition position, ItemID id);

    WorldGenType getType() { return m_type; }
    int64_t getSeed() { return m_seed; }
};