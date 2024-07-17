#pragma once
#include <vector>
#include <raylib.h>
#include <SerializedObject.hpp>

#include "Block.hpp"

#define CHUNK_SIZE 16

class Player;
class Chunk;
class WorldGen;
class Human;

class World : public SerializedObject {
private:
    std::vector<Chunk*> m_chunks;
    std::vector<Rectangle> m_hitboxes;
    std::vector<Human*> m_humans;
    WorldGen* m_WorldGen;

    int m_width;
    int m_height;

    Player* m_player;

public:
    World(int width, int height);
    ~World();

    void generate(WorldGen* generator);
    void update();
    void draw(bool debug);
    bool isBlockClosed(int x, int y);
    void setBlock(int x, int y, Block* block);

    void calcLightDepths();

    void placeBlock(int x, int y, enum Block::BlockType id);
    void destroyBlock(int x, int y);

    void save();

    Block *getBlock(int x, int y);
    Chunk* getChunk(int position);

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);

    int getWidth() {
        return m_width;
    }

    int getHeight() {
        return m_height;
    }

    Player* getPlayer() {
        return m_player;
    }

    WorldGen* getGenerator() {
        return m_WorldGen;
    }
};  
