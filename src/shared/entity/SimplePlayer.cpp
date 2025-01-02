#include "SimplePlayer.hpp"
#include <Block.hpp>
#include <World.hpp>
#include <Chunk.hpp>
#include <Entity.hpp>

SimplePlayer::SimplePlayer(std::shared_ptr<World> world) : Entity::Entity(world) {
    m_header = Header::PLAYER;
}

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

ByteVector SimplePlayer::serialize() {
    SerializedObject::serialize();

    add(m_id);
    add(m_hitbox.x);
    add(m_hitbox.y);
    add(m_animFrame);
    add(m_direction);

    return bytes();
}

size_t SimplePlayer::deserialize(ByteVector const& bytes) {
    std::lock_guard<std::mutex> guard(mutex);
    SerializedObject::deserialize(bytes);

    m_id = get<PlayerID>(0);
    m_hitbox.x = get<float>(0.0f);
    m_hitbox.y = get<float>(0.0f);
    m_direction = get<Direction>(Direction::LEFT);
    m_animFrame = get<uint8_t>(0);

    return m_offset;
}