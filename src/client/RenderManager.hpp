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
    void drawTile(std::string const& mapKey, uint16_t index, Rectf dest, Col4u color = COL_WHITE, float rot = 0.f, Vec2f origin = {0, 0});
    void drawText(std::string const& font, std::string const& text, Vec2f pos, Col4u color = COL_WHITE, float fontSize = 16.f, float spacing = 1.f);
    void drawRect(Rectf rect, Col4u col);
    void drawRectLines(Rectf rect, Col4u col, float thick);

    Vec2f getTextSize(std::string const& text, std::string const& font, float fontSize);

    void renderWorld(std::shared_ptr<World> world, std::shared_ptr<Player> player);
    void renderChunk(std::shared_ptr<Chunk> chunk, std::shared_ptr<Player> player);
    void renderBlock(Rectf dest, std::shared_ptr<Block> block, uint8_t alpha = 255);
    void renderEntity(std::string& textureKey, std::shared_ptr<Entity> entity);
    void renderSimplePlayer(std::shared_ptr<SimplePlayer> player);
    void renderPlayerTexture(Vec2f pos, std::string const& key = "player.png", Vec2f size = {16.f, 22.f}, int animFrame = 0, Direction dir = LEFT, std::string const& username = "", float nameHeight = 11.f, float nameSpacing = 1.f);
};