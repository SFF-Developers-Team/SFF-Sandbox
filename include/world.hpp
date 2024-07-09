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

    int m_width;
    int m_height;

    World(int width, int height);

    void generate();
    void draw(Vector2 playerPosition, int renderDistance, bool debug);
    void BlockCheck();
    bool isBlockAccesible(int x, int y);

    std::vector<Vector2> hitboxes;
};  
