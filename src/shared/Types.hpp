#pragma once

#include <Vector2.hpp>
#include <cstdint>
#include <functional>

struct Vector2i {
    int x;
    int y;

    bool operator==(Vector2i const& other) const {
        return x == other.x && y == other.y;
    }
};

struct Vector2iHash {
    std::size_t operator()(const Vector2i& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};


typedef enum : uint8_t {
    BLOCK_ID_AIR,
    BLOCK_ID_GRASS,
    BLOCK_ID_DIRT,
    BLOCK_ID_STONE,
    BLOCK_ID_COBBLESTONE,
    BLOCK_ID_PLANKS,
    BLOCK_ID_WOOL,
    BLOCK_ID_ADMINIUM, // :D
    BLOCK_ID_BRICKS,
    BLOCK_ID_WOOD_1,
    BLOCK_ID_WOOD_2,
    BLOCK_ID_LEAVES,
    BLOCK_ID_BOOKSHELF,
    BLOCK_ID_FLOWER_POT,
    BLOCK_ID_FURNACE_1,
    BLOCK_ID_FURNACE_2,
    BLOCK_ID_WORKBENCH,
    BLOCK_ID_CHEST,
    BLOCK_ID_BED_1,
    BLOCK_ID_BED_2,
    BLOCK_ID_DEAD_ROSE,
    BLOCK_ID_ROSE,
    BLOCK_ID_IRON_ORE,
    BLOCK_ID_COAL_ORE,
    BLOCK_ID_DIAMOND_ORE,
    BLOCK_ID_GOLD_ORE,
    BLOCK_ID_TORCH_1,
    BLOCK_ID_TORCH_2,
    BLOCK_ID_TORCH_3,
    BLOCK_ID_TORCH_4,
    BLOCK_ID_TORCH_5,
    BLOCK_ID_TORCH_6,
    BLOCK_ID_TORCH_7,
    BLOCK_ID_TORCH_8
} BlockID;

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