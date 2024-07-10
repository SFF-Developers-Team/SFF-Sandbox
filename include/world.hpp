#pragma once
#include <vector>
#include <raylib.h>

#include "Block.hpp"

#define CHUNK_SIZE 16

class Chunk;

class World {
public:
    std::vector<Block*> m_blocks;
    std::vector<Chunk*> m_chunks;
    std::vector<Vector2> m_hitboxes;

    int m_width;
    int m_height;

    int m_playerInChunk;
    int m_renderMinX = -1;
    int m_renderMaxX = -1;

    World(int width, int height);

    void generate();
    void update(Vector2 playerPosition, int renderDistance);
    void draw(bool debug);
    void buildHitboxes();
    bool isBlockAccesible(int x, int y);

    void placeBlock(int x, int y, enum Block::BlockType id);
    void destroyBlock(int x, int y);
};  
