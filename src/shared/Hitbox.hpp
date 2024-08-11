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

    float clipXCollide(Hitbox& otherHitbox, float x);
    float clipYCollide(Hitbox& otherHitbox, float x);

    bool intersects(Hitbox& otherHitbox);
    void move(float x, float y);
    Hitbox expand(float x, float y);

    Rectf getRect() { return Rectf {x, y, width, height }; }
};