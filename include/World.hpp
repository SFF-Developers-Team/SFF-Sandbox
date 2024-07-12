#pragma once
#include <vector>
#include <raylib.h>

#include "Block.hpp"

#define CHUNK_SIZE 16

class Player;
class Chunk;

class World {
private:
    std::vector<Chunk*> m_chunks;
    std::vector<Rectangle> m_hitboxes;

    int m_width;
    int m_height;

    Player* m_player;

public:
    World(int width, int height);
    ~World();

    void generate();
    void update(int renderDistance);
    void draw(bool debug);
    bool isBlockClosed(int x, int y);
    void setBlock(int x, int y, Block* block);

    void calcLightDepths();

    void placeBlock(int x, int y, enum Block::BlockType id);
    void destroyBlock(int x, int y);

    Block *getBlock(int x, int y);
    Chunk* getChunk(int position);

    int getWidth() {
        return m_width;
    }

    int getHeight() {
        return m_height;
    }

    Player* getPlayer() {
        return m_player;
    }
};  
