#pragma once

#include "Item.hpp"
#include <Vector2.hpp>
#include <cstdint>
#include <functional>
#include <memory>

struct Vector2i {
    int x;
    int y;

    Vector2i() : x(0), y(0) {}
    Vector2i(int x, int y) : x(x), y(y) {}
    Vector2i(raylib::Vector2 const& v) : x(v.x), y(v.y) {}

    operator raylib::Vector2() { return raylib::Vector2(x, y); }

    bool operator==(Vector2i const& other) const {
        return x == other.x && y == other.y;
    }
};

struct Vector2iHash {
    std::size_t operator()(const Vector2i& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};


typedef enum : int8_t {DIRECTION_RIGHT = -1, DIRECTION_LEFT = 1} Direction;

enum PlayerAnimationType {
    PLAYER_ANIMATION_TYPE_IDLE,
    PLAYER_ANIMATION_TYPE_WALK,
    PLAYER_ANIMATION_TYPE_SNEAK,
    PLAYER_ANIMATION_TYPE_JUMP,
    PLAYER_ANIMATION_TYPE_HIT,
    PLAYER_ANIMATION_TYPE_HURT,
    PLAYER_ANIMATION_TYPE_SIT,
    PLAYER_ANIMATION_TYPE_CART
};

enum GameMode {
    GAMEMODE_SURVIVAL,
    GAMEMODE_CREATIVE
};

enum ToolType {
    TYPE_SWORD,
    TYPE_PICKAXE,
    TYPE_AXE,
    TYPE_SHOVEL,
    TYPE_HOE,
    TYPE_SCISSORS,
    TYPE_ALL,
    TYPE_NONE
};

enum ItemFactoryID {
    ID_BLOCKITEM_GRASS,
    ID_BLOCKITEM_DIRT,
    ID_BLOCKITEM_STONE,
    ID_BLOCKITEM_COBBLESTONE,
    ID_BLOCKITEM_WOODEN_PLANKS,
    ID_BLOCKITEM_WOOL,
    ID_BLOCKITEM_ADMINIUM,
    ID_BLOCKITEM_BRICK,
    ID_BLOCKITEM_WOOD_1,
    ID_BLOCKITEM_WOOD_2,
    ID_BLOCKITEM_LEAVES,
    ID_BLOCKITEM_BOOKSHELF,
    ID_BLOCKITEM_FLOWER_POT,
    ID_BLOCKITEM_FURNACE,
    ID_BLOCKITEM_WORKBENCH,
    ID_BLOCKITEM_CHEST,
    ID_BLOCKITEM_BED,
    ID_BLOCKITEM_DEAD_ROSE,
    ID_BLOCKITEM_ROSE,
    ID_BLOCKITEM_IRON_ORE,
    ID_BLOCKITEM_COAL_ORE,
    ID_BLOCKITEM_DIAMOND_ORE,
    ID_BLOCKITEM_GOLD_ORE,
    ID_BLOCKITEM_TORCH,

    ID_FOOD_POTATO,
    ID_FOOD_APPLE,
};

struct BlockInfo {
    float hardness;
    ToolType toolType;
    ItemFactoryID fabricID;
};

struct InventoryItem {
    std::shared_ptr<Item> item;
    int count;
};

struct ItemInfo {
    ToolType type;
    uint8_t level;
    uint8_t spriteIndex;
    float speed;
    std::string name;
};

struct BlockPosition {
    float x;
    float y;
    int z;
};