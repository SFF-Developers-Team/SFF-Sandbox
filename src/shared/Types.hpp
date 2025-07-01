#pragma once

#include <Vector2.hpp>
#include <cstdint>
#include <functional>

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