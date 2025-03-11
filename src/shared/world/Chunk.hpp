#pragma once
#include <world/gen/WorldGen.hpp>
#include <world/Block.hpp>
#include <memory>
#include <vector>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_DEPTH 2

class Chunk : public Serializable {
private:
    std::vector<std::shared_ptr<Block>> m_blocks;

public:
    Chunk();
    
    void setBlock(int x, int y, uint8_t layer, ItemID type);
    void setBlock(int x, int y, uint8_t layer, std::shared_ptr<Block> block);
    std::shared_ptr<Block> getBlock(int x, int y, uint8_t layer);

    int getIndex(int x, int y, uint8_t layer);

    // Is block closed by other blocks in 4 directions
    bool isBlockClosed(int x, int y, uint8_t layer);
    bool isOutOfBound(int x, int y, uint8_t layer);

    DataStream serialize() override;
    bool deserialize(DataStream& stream) override;

    uint16_t countBlocks();
};