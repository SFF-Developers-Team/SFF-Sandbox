#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <SerializedObject.hpp>
#include <unordered_map>
#include <string>

class World;
class Player : public SerializedObject {
public:
    enum AnimationType {
        PLAYER_IDLE,
        PLAYER_MOVE,
        PLAYER_SNEAK,
        PLAYER_JUMP,
        PLAYER_HIT,
        PLAYER_HURT,
        PLAYER_SIT,
        PLAYER_CART
    };

private:
    Rectangle m_hitbox;
    Texture2D m_texture;
    Vector2 m_speed = {0.0f, 0.0f};
    World* m_world;
    Camera2D m_camera;

    AnimationType m_animType = PLAYER_IDLE;
    int m_animFps = 8;
    int m_animCurrentFrame = 0;
    double m_animLastFrameTime = 0.0f;

    char m_direction = 1;
    bool m_canJump = false;
    bool m_fly = false;

public:
    Player(World* world);
    ~Player();
    
    void update(std::vector<Rectangle> envHitboxes); 
    void draw();
    void updateCamera();
    void updateAnimation();

    void setAnimation(AnimationType type);
    static const char* getAnimationName(AnimationType type);
    inline Vector2 getPosition() {
        return Vector2 {m_hitbox.x, m_hitbox.y};
    }

    Camera2D getCamera() {
        return m_camera;
    }

    SObject encodeObject() override;
    int decodeObject(SObject &s) override;

    Vector2 convertToCameraPos(Vector2 pos);
    Vector2 getTargetBlock(bool onlyExist = true);
};
