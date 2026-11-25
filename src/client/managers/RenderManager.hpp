#pragma once
#include <memory>
#include <string>
#include <Types.hpp>
// #include <Item.hpp>
#include <Rectangle.hpp>
#include <Color.hpp>

class World;
class LocalPlayer;
class Entity;
class Player;
class InventorySlot;
class Item;

namespace RenderManager {
    void drawRectLines(raylib::Rectangle rect, raylib::Color col, float thick = 0.f);
    void drawFrame(raylib::Rectangle rect, raylib::Color col, float borderSize);
    void renderInventoryItem(raylib::Rectangle dest, ItemID itemId, int count = 1);
    void renderEntity(std::string& textureKey, std::shared_ptr<Entity> entity);
    void renderPlayerTexture(raylib::Vector2 pos, std::string const& key = "player.png", raylib::Vector2 size = {16.f, 22.f}, int animFrame = 0, Direction dir = LEFT, std::string const& username = "", float nameHeight = 11.f, float nameSpacing = 1.f);
};