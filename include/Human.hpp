#pragma once
#include <Entity.hpp>
#include <map>

class Human : public Entity {
public:
    enum AnimationType : unsigned char {
        PLAYER_IDLE,
        PLAYER_MOVE,
        PLAYER_SNEAK,
        PLAYER_JUMP,
        PLAYER_HIT,
        PLAYER_HURT,
        PLAYER_SIT,
        PLAYER_CART
    };

    const static inline std::map<AnimationType, std::pair<uint8_t, uint8_t>> m_animLimits = {
        {PLAYER_MOVE, {1, 5}},
        {PLAYER_SNEAK, {6, 7}},
        {PLAYER_HIT, {9, 13}}
    };

protected:
    AnimationType m_animType = PLAYER_IDLE;

    int m_animFps = 10;
    unsigned char m_animCurrentFrame = 0;
    float m_animLastFrameTime = 0.0f;

    float m_lastActionTime = 0.0f;

    char m_direction = 1;
    bool m_canJump = false;

    static inline Texture2D m_texture;

public:
    Human(World* world);
    ~Human();
    
    void update(); 
    void updateAnimation();
    void processPhysics(bool hitWall, bool hitFloor, bool hitCeil) override;
    void draw();

    unsigned char animationClamp(unsigned char value, unsigned char min, unsigned char max);

    void setAnimation(AnimationType type);
    static const char* getAnimationName(AnimationType type);
};