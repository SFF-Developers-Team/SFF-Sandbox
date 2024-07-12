#pragma once
#include <vector>
#include <raylib.h>

#include "Block.hpp"

#define CHUNK_SIZE 16

class Player;

class World {
private:
    std::vector<Block*> m_blocks;
    std::vector<Rectangle> m_hitboxes;
    float* m_lightDepths;

    int m_width;
    int m_height;

    int m_playerInChunk;
    int m_renderMinX = -5;
    int m_renderMaxX = -1;

    Player* m_player;

public:
    World(int width, int height);
    ~World();

    void generate();
    void update(int renderDistance);
    void draw(bool debug);
    bool isBlockAccesible(int x, int y);
    void setBlock(int x, int y, Block* block);

    void calcLightDepths();

    void placeBlock(int x, int y, enum Block::BlockType id);
    void destroyBlock(int x, int y);

    Block *getBlock(int x, int y);

    int getWidth() {
        return m_width;
    }

    int getHeight() {
        return m_height;
    }

    int getPlayerChunk() {
        return m_playerInChunk;
    }

    // in blocks
    Vector2 getRenderLimits() {
        return Vector2 {(float)m_renderMinX, (float)m_renderMaxX};
    }

    Player* getPlayer() {
        return m_player;
    }

    std::vector<Block*> getBlocks() {
        return m_blocks;
    }
};  
