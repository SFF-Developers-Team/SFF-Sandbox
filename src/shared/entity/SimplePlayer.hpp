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

    const static inline std::map<AnimationType, std::pair<uint8_t, uint8_t>> m_animLimits = {
        {PLAYER_MOVE, {1, 5}},
        {PLAYER_SNEAK, {6, 7}},
        {PLAYER_HIT, {9, 13}}
    };

protected:
    AnimationType m_animType = PLAYER_IDLE;
    uint8_t m_animFps = 10;
    uint8_t m_animCurrentFrame = 0;
    float m_animLastFrameTime = 0.0f;
    float m_lastActionTime = 0.0f;

    // multiplayer features
    PlayerID m_id;
    std::string m_username;

public:
    SimplePlayer(World* world);
    ~SimplePlayer();

    uint8_t animationClamp(uint8_t value, uint8_t min, uint8_t max);
    void setAnimation(AnimationType type);
    static const char* getAnimationName(AnimationType type);

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);

    static std::size_t const getSizeBytes();

    void setUsername(std::string const& username) { m_username = username; }
    void setAnimCurrentFrame(uint8_t frame) { m_animCurrentFrame = frame; }
    void setID(PlayerID id) { m_id = id; }

    auto getAnimCurrentFrame() { return m_animCurrentFrame; }
    auto getUsername() { return m_username; }
    auto getID() { return m_id; }
};