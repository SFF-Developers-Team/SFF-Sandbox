#pragma once
#include <vector>
#include <raylib.h>

#include "Block.hpp"

#define CHUNK_SIZE 16

class Chunk;

class World {
public:
    Block* m_blocks;
    std::vector<Chunk*> m_chunks;
    std::vector<Vector2> m_hitboxes;

    int m_width;
    int m_height;

    int m_playerInChunk;
    int m_renderMinX = 0;
    int m_renderMaxX = 0;

    World(int width, int height);

    void generate();
    void update(Vector2 playerPosition, int renderDistance);
    void draw(bool debug);
    void buildHitboxes();
    bool isBlockAccesible(int x, int y);

    std::vector<Vector2> hitboxes;
};  
