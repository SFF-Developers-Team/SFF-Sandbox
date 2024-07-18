#pragma once
#include <vector>
#include <raylib.h>
#include <SerializedObject.hpp>
#include <Particle.hpp>
#include <Block.hpp>

#define CHUNK_SIZE 16

class Player;
class Chunk;
class WorldGen;
class Human;
class Particle;

class World : public SerializedObject {
private:
    std::vector<Chunk*> m_chunks;
    std::vector<Rectangle> m_hitboxes;
    std::vector<Human*> m_humans;
    std::vector<Particle*> m_particles;
    WorldGen* m_WorldGen;

    int m_width;
    int m_height;

    Player* m_player;

public:
    World(int width, int height);
    ~World();

    void generate(WorldGen* generator);
    void update();
    void draw();
    bool isBlockClosed(int x, int y, uint8_t layer);
    void setBlock(int x, int y, uint8_t layer, Block* block);

    void calcLightDepths();

    void placeBlock(int x, int y, uint8_t layer, enum Block::BlockType id);
    void destroyBlock(int x, int y, uint8_t layer);

    bool save();
    bool load();

    Block* getBlock(int x, int y, uint8_t layer);
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