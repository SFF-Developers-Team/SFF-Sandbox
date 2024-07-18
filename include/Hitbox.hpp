#pragma once
#include <raylib.h>

class Hitbox {
private:
    Rectangle m_rect;

public:
    Hitbox(Rectangle rect);
    Hitbox(float x, float y, float width, float height);

    void expand(float width, float height);
    void move(float x, float y);

    bool checkXCollide(Hitbox& hitbox);
    bool checkYCollide(Hitbox& hitbox);
};