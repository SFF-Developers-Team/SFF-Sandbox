#pragma once
#include <Block.hpp>
#include <vector>
#include <raylib.h>
#include <WorldGen.hpp>

#define CHUNK_WIDTH 16

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

    void setBlock(int x, int y, Block::BlockType type);
    void setBlock(int x, int y, Block* block);
    void setBlock(Vector2 pos, Block* block);
    void setBlock(Vector2 pos, Block::BlockType type);

    Block* getBlock(int x, int y);
    Block* getBlock(Vector2 pos);

    int getHeight();

    void resetLightDepts();
    void setLightDepth(int x, int y, float d);
    float getLightDepth(int x, int y);

    // Is block closed by other blocks in 4 directions
    bool isBlockClosed(int x, int y);
    bool isBlockClosed(Vector2 pos);

    bool isOutOfBound(int x, int y);

    void generate();
    void draw();

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);
};