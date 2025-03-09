#pragma once
#include <world/gen/WorldGen.hpp>
#include <world/Block.hpp>
#include <memory>
#include <vector>

#define CHUNK_WIDTH 16
#define LAYERS 2

class World;

class Chunk : public SerializedObject {
public:
    typedef int32_t Position;

private:
    std::vector<std::shared_ptr<Block>> m_blocks;
    std::shared_ptr<World> m_world;
    Position m_position;

public:
    Chunk(std::shared_ptr<World> world, Position position = 0);

    void onTick();

    auto getPosition() { return m_position; }
    auto getWorld() { return m_world; }

    void setBlock(int x, int y, uint8_t layer, ItemID type);
    void setBlock(int x, int y, uint8_t layer, std::shared_ptr<Block> block);
    std::shared_ptr<Block> getBlock(int x, int y, uint8_t layer);

    int getIndex(int x, int y, uint8_t layer);

    // Is block closed by other blocks in 4 directions
    bool isBlockClosed(int x, int y, uint8_t layer);

    bool isOutOfBound(int x, int y, uint8_t layer);

    ByteVector serialize() override;
    size_t deserialize(ByteVector const& bytes) override;

    uint16_t countBlocks();
};