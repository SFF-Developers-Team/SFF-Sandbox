#include "SimplePlayer.hpp"
#include <Block.hpp>
#include <World.hpp>
#include <Chunk.hpp>

#define WALK_SPEED 150
#define JUMP_SPEED 200
#define G 400

SimplePlayer::SimplePlayer(World* world, bool enablePhysics) : Entity::Entity(world, {0, 0}, enablePhysics) {    
    m_header = Header::PLAYER;
    m_hitbox = {(float)(rand() % m_world->getWidth() * BS), 0, 25.f, 44.f};
    m_enabledPhysics = true;
    m_gravitation = 400.f;
}

SimplePlayer::SimplePlayer(World* world) : SimplePlayer(world, true) {}

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

unsigned char SimplePlayer::animationClamp(unsigned char value, unsigned char min, unsigned char max) {
    if(value > max) return min;
    if(value < min) return min;
    return value;
}

void SimplePlayer::update() {
    Entity::update();
}

void SimplePlayer::processPhysics(bool hitWall, bool hitFloor, bool hitCeil) {
    if(!hitFloor) {
        setAnimation(PLAYER_JUMP);
        m_canJump = false;
    } else {
        m_canJump = true;
    }
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
    m_hitbox.x = getBytes<float>(rand() % m_world->getWidth() * BS);
    m_hitbox.y = getBytes<float>(0.0f);
    m_animCurrentFrame = getBytes<uint8_t>(0);
    m_direction = getBytes<int8_t>(1);

    return m_offset;
}

size_t const SimplePlayer::getSize() {
    return sizeof(Header) + sizeof(PlayerID) + (sizeof(float) * 2) + (sizeof(uint8_t) * 2);
}