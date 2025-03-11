#pragma once
#include <Types.hpp>

// Inspired by RubyDung's AABB
class Hitbox {
public:
    float x;
    float y;
    float width;
    float height;
    
    Hitbox(Rectf rec);
    Hitbox(float x, float y, float width, float height);

    float clipXCollide(Hitbox const& otherHitbox, float x);
    float clipYCollide(Hitbox const& otherHitbox, float x);

    bool intersects(Hitbox const& otherHitbox);
    void move(float x, float y);
    Hitbox expand(float x, float y);

    Rectf getRect() { return Rectf {x, y, width, height }; }
};