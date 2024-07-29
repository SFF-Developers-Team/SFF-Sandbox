#pragma once
#include <raylib.h>

class World;
class Player;
class Entity;
class Chunk;
class SimplePlayer;

class RenderManager {
private:
    World* m_world;
    Player* m_player;
    Texture2D m_texture; // Временно

public:
    RenderManager(World* world, Player* player);

    void renderWorld();
    void renderChunk(Chunk* chunk);
    void renderEntity(Entity* entity);
    void renderSimplePlayer(SimplePlayer* player);
};