#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <SerializedObject.hpp>

class World;
class Player : public SerializedObject {
private:
    Rectangle m_rect;
    Texture2D m_texture;

    Vector2 cursor;
    Vector2 cameraCursor;

    Vector2 m_speed = {0.0f, 0.0f};
    char m_direction = 1;
    bool m_canJump = false;
    bool m_fly = false;

public:
    World* m_world;
    Camera2D camera;
    Vector2 cameraCursor2;
    Player(World* world);
    
    void update(std::vector<Rectangle> envHitboxes); 
    void draw();
    void updateCamera();
    void blockBreak();
    
    inline Vector2 getPosition() {
        return Vector2 {m_rect.x, m_rect.y};
    }

    SObject encodeObject() override;
    int decodeObject(SObject &s) override;

};