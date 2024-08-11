#pragma once
#include <Block.hpp>
#include <vector>
#include <WorldGen.hpp>
#include <memory>

#define CHUNK_WIDTH 16
#define LAYERS 2

using ChunkPosition = int32_t;

class World;

class Chunk : public SerializedObject {
private:
    std::vector<std::unique_ptr<Block>> m_blocks;
    World* m_world;
    ChunkPosition m_position;

public:
    Chunk(World* world, ChunkPosition position);
    Chunk(World* world);
    ~Chunk();
    
    ChunkPosition getPosition() {
        return m_position;
    }

    void setBlock(int x, int y, uint8_t layer, Block::BlockType type);
    void setBlock(int x, int y, uint8_t layer, std::unique_ptr<Block> block);
    void setBlock(Vec2i pos, uint8_t layer, std::unique_ptr<Block> block);
    void setBlock(Vec2i pos, uint8_t layer, Block::BlockType type);

    Block* getBlock(int x, int y, uint8_t layer);
    Block* getBlock(Vec2i pos, uint8_t layer);

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

    void generate();
    void draw();

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);
};