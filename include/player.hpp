#pragma once

#include <raylib.h>
#include <vector>

class Player {
private:
    Rectangle m_rect;
    Texture2D m_texture;

    int m_speed = 3.0f;
    bool m_canJump = true;

public:
    Camera2D camera;

    Player();
 
    void update(std::vector<Vector2> envHitboxes); 
    void draw();
    void updateCamera();

    inline Vector2 getPosition() {
        return Vector2 {m_rect.x, m_rect.y};
    }
};