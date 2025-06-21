#pragma once

#include <world/Block.hpp>
#include <memory>

class World;
class Chunk;

class WorldGen {
public:
    enum Type : uint8_t {
        NORMAL
    };

protected:
    std::shared_ptr<World> m_world;
    int64_t m_seed = 0;
    Type m_type;

public:
    WorldGen(std::shared_ptr<World> world, uint64_t seed);

    virtual std::shared_ptr<Chunk> generateChunk(int32_t position) = 0;

    void generateBlock(std::shared_ptr<Chunk> chunk, BlockPosition position, ItemID id);

    Type getType() { return m_type; }
    int64_t getSeed() { return m_seed; }
};