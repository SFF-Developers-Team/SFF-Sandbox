#pragma once
#include <world/gen/WorldGen.hpp>
#include <world/Block.hpp>
#include <memory>
#include <vector>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 2
#define TO_CHUNK_POS(pos) (Vec2i {static_cast<int>(pos.x) / CHUNK_WIDTH, static_cast<int>(pos.y) / CHUNK_HEIGHT})

class Chunk : public Serializable {
private:
    std::vector<std::shared_ptr<Block>> m_blocks;

public:
    Chunk();
    
    void setBlock(BlockPosition pos, std::shared_ptr<Block> block);
    std::shared_ptr<Block> getBlock(BlockPosition pos);

    int getIndex(BlockPosition pos);

    // Is block closed by other blocks in 4 directions
    bool isBlockClosed(BlockPosition pos);
    bool isOutOfBound(BlockPosition pos);

    DataStream serialize() override;
    bool deserialize(DataStream& stream) override;

    uint16_t countBlocks();
};