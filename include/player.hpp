#pragma once

#include <raylib.h>
#include <vector>

class Player {
private:
    Rectangle m_rect;
    Texture2D m_texture;

    Vector2 m_speed = {0.0f, 0.0f};
    char m_direction = 1;
    bool m_canJump = false;
    bool m_fly = false;

public:
    Camera2D camera;

    Player();
 
    void update(std::vector<Rectangle> envHitboxes); 
    void draw();
    void updateCamera();

    inline Vector2 getPosition() {
        return Vector2 {m_rect.x, m_rect.y};
    }
};