#pragma once
#include <memory>
#include <string>
#include <Types.hpp>

class World;
class Player;
class Entity;
class Chunk;
class SimplePlayer;
class Block;

class RenderManager {
public:
    RenderManager();

    static RenderManager* get() {
        static auto rm = new RenderManager();
        return rm;
    }

    void drawTexture(std::string const& key, Rectf dest, Col4u color = COL_WHITE, float rot = 0.f, Vec2f origin = {0, 0});

    void renderWorld(std::shared_ptr<World> world, std::shared_ptr<Player> player);
    void renderChunk(std::shared_ptr<Chunk> chunk, std::shared_ptr<Player> player);
    void renderBlock(float x, float y, std::shared_ptr<Block> block, uint8_t alpha = 255);
    void renderUIBlock(float x, float y, float width, float height, std::shared_ptr<Block> block);
    void renderEntity(std::string& textureKey, std::shared_ptr<Entity> entity);
    void renderSimplePlayer(std::shared_ptr<SimplePlayer> player);
};