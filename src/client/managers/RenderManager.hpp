#pragma once
#include <memory>
#include <string>
#include <Types.hpp>

class World;
class LocalPlayer;
class Entity;
class Chunk;
class Player;
class InventoryItem;
class Block;

namespace RenderManager {
    void drawTexture(std::string const& key, Rectf dest, Col4u color = COL_WHITE, float rot = 0.f, Vec2f origin = {0.f, 0.f});
    void drawTile(std::string const& mapKey, uint16_t index, Rectf dest, Col4u color = COL_WHITE, float rot = 0.f, Vec2f origin = {0.f, 0.f});
    void drawText(std::string const& font, std::string const& text, Vec2f pos, Col4u color = COL_WHITE, float fontSize = 0.f, Vec2f origin = {0.f, 0.f}, float spacing = 1.f);
    void drawColoredText(std::string const& font, std::string const& text, Vec2f pos, Col4u color = COL_WHITE, float fontSize = 0.f, Vec2f origin = {0.f, 0.f}, float spacing = 1.f);
    void drawRect(Rectf rect, Col4u col);
    void drawRectLines(Rectf rect, Col4u col, float thick);
    void drawFrame(Rectf rect, Col4u col, float borderSize);

    Vec2f getTextSize(std::string const& text, std::string const& font, float fontSize = 0.f, float spacing = 1.f);

    void renderBlock(Rectf dest, BlockPosition position, std::shared_ptr<Block> block, uint8_t alpha = 255);
    void renderInventoryItem(Rectf dest, std::shared_ptr<InventoryItem> item);
    void renderEntity(std::string& textureKey, std::shared_ptr<Entity> entity);
    void renderPlayerTexture(Vec2f pos, std::string const& key = "player.png", Vec2f size = {16.f, 22.f}, int animFrame = 0, Direction dir = LEFT, std::string const& username = "", float nameHeight = 11.f, float nameSpacing = 1.f);
};