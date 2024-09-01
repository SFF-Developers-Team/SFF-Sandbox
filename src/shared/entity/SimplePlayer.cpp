#include "SimplePlayer.hpp"
#include <Block.hpp>
#include <World.hpp>
#include <Chunk.hpp>
#include <Entity.hpp>

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
        m_animFrame = animationClamp(m_animFrame, limit.first, limit.second);
    }
}

uint8_t SimplePlayer::animationClamp(uint8_t value, uint8_t min, uint8_t max) {
    if(value > max || value < min) return min;
    return value;
}

ByteVector& SimplePlayer::serialize() {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::serialize();

    addBytes(m_id);
    addBytes(m_hitbox.x);
    addBytes(m_hitbox.y);
    addBytes(m_animFrame);
    addBytes(m_direction);

    return m_bytes;
}

size_t SimplePlayer::deserialize(ByteVector& bytes) {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::deserialize(bytes);

    m_id = getBytes<PlayerID>(0);
    m_hitbox.x = getBytes<float>(0.0f);
    m_hitbox.y = getBytes<float>(0.0f);
    m_direction = getBytes<Direction>(Direction::LEFT);
    m_animFrame = getBytes<uint8_t>(0);

    return m_offset;
}

std::size_t const SimplePlayer::getSizeBytes() {
    return sizeof(Header) + sizeof(PlayerID) + sizeof(float) + sizeof(float) + sizeof(Entity::Direction) + sizeof(uint8_t);
}