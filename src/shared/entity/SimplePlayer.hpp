#pragma once
#include <map>
#include <Entity.hpp>

using PlayerID = uint32_t;

class SimplePlayer : public Entity {
public:
    enum AnimationType : uint8_t {
        PLAYER_IDLE,
        PLAYER_MOVE,
        PLAYER_SNEAK,
        PLAYER_JUMP,
        PLAYER_HIT,
        PLAYER_HURT,
        PLAYER_SIT,
        PLAYER_CART
    };

    enum Direction : int8_t {
        LEFT = -1,
        RIGHT = 1
    };

    const static inline std::map<AnimationType, std::pair<uint8_t, uint8_t>> m_animLimits = {
        {PLAYER_MOVE, {1, 5}},
        {PLAYER_SNEAK, {6, 7}},
        {PLAYER_HIT, {9, 13}}
    };

protected:
    static inline size_t m_size = 0;

    AnimationType m_animType = PLAYER_IDLE;

    uint8_t m_animFps = 10;
    uint8_t m_animCurrentFrame = 0;
    float m_animLastFrameTime = 0.0f;

    float m_lastActionTime = 0.0f;

    int8_t m_direction = 1;
    bool m_canJump = false;

    // multiplayer features
    PlayerID m_id;
    std::string m_username;

public:
    SimplePlayer();
    SimplePlayer(World* world);
    SimplePlayer(World* world, bool enablePhysics);
    ~SimplePlayer();
    
    virtual void update() override;
    void processPhysics(bool hitWall, bool hitFloor, bool hitCeil) override;

    uint8_t animationClamp(uint8_t value, uint8_t min, uint8_t max);

    void setAnimation(AnimationType type);
    static const char* getAnimationName(AnimationType type);

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);

    static size_t const getSize();

    void setUsername(std::string const& username) { m_username = username; }
    void setAnimCurrentFrame(uint8_t frame) { m_animCurrentFrame = frame; }
    void turn(SimplePlayer::Direction direction) { m_direction = direction; }
    void setID(PlayerID id) { m_id = id; }

    auto getAnimCurrentFrame() { return m_animCurrentFrame; }
    auto getDirection() { return m_direction; }
    auto getUsername() { return m_username; }
    auto getID() { return m_id; }
};