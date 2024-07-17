#pragma once
#include <Block.hpp>
#include <vector>
#include <raylib.h>
#include <WorldGen.hpp>

#define CHUNK_WIDTH 16
#define LAYERS 2

class World;

class Chunk : public SerializedObject {
private:
    std::vector<Block*> m_blocks;
    float* m_lightDepths;
    World* m_world;
    int m_position;

public:
    Chunk(World* world, int position);
    Chunk(World* world);
    ~Chunk();
    
    int getPosition() {
        return m_position;
    }

    void setBlock(int x, int y, int layer, Block::BlockType type);
    void setBlock(int x, int y, int layer, Block* block);
    void setBlock(Vector2 pos, int layer, Block* block);
    void setBlock(Vector2 pos, int layer, Block::BlockType type);

    Block* getBlock(int x, int y, int layer);
    Block* getBlock(Vector2 pos, int layer);

    int getHeight();
    int getIndex(int x, int y, int layer);

    void resetLightDepts();
    void setLightDepth(int x, int y, float d);
    float getLightDepth(int x, int y);

    // Is block closed by other blocks in 4 directions
    bool isBlockClosed(int x, int y, int layer);
    bool isBlockClosed(Vector2 pos, int layer);

    bool isOutOfBound(int x, int y, int layer);
    bool isOutOfBound(Vector2 pos, int layer);

    void generate();
    void draw();

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);
};