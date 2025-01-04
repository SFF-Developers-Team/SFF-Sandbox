#pragma once
#include <Block.hpp>
#include <vector>
#include <WorldGen.hpp>
#include <memory>

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
    
    Position getPosition() {
        return m_position;
    }

    void setBlock(int x, int y, uint8_t layer, Block::ID type);
    void setBlock(int x, int y, uint8_t layer, std::shared_ptr<Block> block);
    void setBlock(Vec2i pos, uint8_t layer, std::shared_ptr<Block> block);
    void setBlock(Vec2i pos, uint8_t layer, Block::ID type);

    std::shared_ptr<Block> getBlock(int x, int y, uint8_t layer);
    std::shared_ptr<Block> getBlock(Vec2i pos, uint8_t layer);

    int getHeight();
    int getIndex(int x, int y, uint8_t layer);

    void resetLightDepts();
    void setLightDepth(int x, int y, float d);
    float getLightDepth(int x, int y);

    // Is block closed by other blocks in 4 directions
    bool isBlockClosed(int x, int y, uint8_t layer);
    bool isBlockClosed(Vec2i pos, uint8_t layer);

    bool isOutOfBound(int x, int y, uint8_t layer);
    bool isOutOfBound(Vec2i pos, uint8_t layer);

    void draw();

    ByteVector serialize() override;
    size_t deserialize(ByteVector const& bytes) override;

    uint16_t countBlocks();
};