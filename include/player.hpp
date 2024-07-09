#pragma once

#include <raylib.h>

class Player {
private:
    Rectangle m_rect;
    Texture2D m_texture;

    int playerSpeed = 3.0f;

public:
    Camera2D camera;

    Player();
 
    void update(); 
    void draw();
    void updateCamera();

    inline Vector2 getPosition() {
        return Vector2 {m_rect.x, m_rect.y};
    }
};