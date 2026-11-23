#pragma once
#include <Types.hpp>

// Inspired by RubyDung's AABB
class Hitbox {
public:
    float x;
    float y;
    float width;
    float height;
    
    Hitbox(Hitbox& hitbox) = default;
    Hitbox(Rectf rec);
    Hitbox(float x, float y, float width, float height);

    float clipXCollide(Hitbox const& otherHitbox, float xa);
    float clipYCollide(Hitbox const& otherHitbox, float ya);

    bool intersects(Hitbox const& otherHitbox);
    void move(float x, float y);
    Hitbox expand(float x, float y);

    operator Rectf() const { return Rectf {x, y, width, height }; }

#ifdef RL_RECTANGLE_TYPE
    operator raylib::Rectangle() const { return raylib::Rectangle {x, y, width, height }; }
#endif
};