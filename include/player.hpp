#pragma once

#include <raylib.h>

class Player {
private:
Vector2 position;
Vector2 playersize = {35,42};
Rectangle hitbox;
Texture2D player;
public:
    Player();
    void update(); 
};