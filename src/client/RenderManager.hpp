#pragma once
#include <raylib.h>
#include <memory>

class World;
class Player;
class Entity;
class Chunk;
class SimplePlayer;
class Block;

class RenderManager {
private:
    std::shared_ptr<Player> m_player; 
    World* m_world;
    Texture2D m_texture; // Временно

public:
    RenderManager(World* world, std::shared_ptr<Player> player);

    void renderWorld();
    void renderChunk(std::shared_ptr<Chunk> chunk);
    void renderBlock(float x, float y, std::shared_ptr<Block> block);
    void renderSelectedBlock(float x, float y, std::shared_ptr<Block> block);
    void renderEntity(Entity* entity);
    void renderSimplePlayer(std::shared_ptr<SimplePlayer> player);
};