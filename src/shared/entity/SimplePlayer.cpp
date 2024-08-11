#include "SimplePlayer.hpp"
#include <Block.hpp>
#include <World.hpp>
#include <Chunk.hpp>

#define WALK_SPEED 15
#define JUMP_SPEED 20

SimplePlayer::SimplePlayer(World* world) : Entity::Entity(world) {
    m_header = Header::PLAYER;
}

SimplePlayer::~SimplePlayer() {}

// for debug
const char* SimplePlayer::getAnimationName(AnimationType type) {
    switch(type) {
        case PLAYER_IDLE:
            return "PLAYER_IDLE";
        case PLAYER_MOVE:
            return "PLAYER_MOVE";
        case PLAYER_SNEAK:
            return "PLAYER_SNEAK";
        case PLAYER_JUMP:
            return "PLAYER_JUMP";
        case PLAYER_HIT:
            return "PLAYER_HIT";
        case PLAYER_HURT:
            return "PLAYER_HURT";
        case PLAYER_SIT:
            return "PLAYER_SIT";
        case PLAYER_CART:
            return "PLAYER_CART";
    }
}

void SimplePlayer::setAnimation(AnimationType type) {
    m_animType = type;

    if(m_animLimits.count(type)) {
        auto limit = m_animLimits.at(type);
        m_animCurrentFrame = animationClamp(m_animCurrentFrame, limit.first, limit.second);
    }
}

uint8_t SimplePlayer::animationClamp(uint8_t value, uint8_t min, uint8_t max) {
    if(value > max || value < min) return min;
    return value;
}


ByteVector& SimplePlayer::serialize() {
    SerializedObject::serialize();

    addBytes(m_id);
    addBytes(m_hitbox.x);
    addBytes(m_hitbox.y);
    addBytes(m_animCurrentFrame);
    addBytes(m_direction);

    return m_bytes;
}

int SimplePlayer::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);

    m_id = getBytes<PlayerID>(0);
    m_hitbox.x = getBytes<float>(0.0f);
    m_hitbox.y = getBytes<float>(0.0f);
    m_direction = getBytes<Direction>(Direction::LEFT);
    m_animCurrentFrame = getBytes<uint8_t>(0);

    return m_offset;
}

<<<<<<< HEAD
std::size_t const SimplePlayer::getSize() {
    return sizeof(Header) + sizeof(PlayerID) + (sizeof(float) * 2) + (sizeof(uint8_t) * 2);
=======
size_t const SimplePlayer::getSizeBytes() {
    return sizeof(PlayerID) + sizeof(float) + sizeof(float) + sizeof(Direction) + sizeof(uint8_t);
>>>>>>> 2f9760e2af06782b6f2bdc5eb399f07f065f1f12
}