#include <entity/SimplePlayer.hpp>
#include <entity/Entity.hpp>
#include <world/Block.hpp>
#include <world/World.hpp>
#include <world/Chunk.hpp>

SimplePlayer::SimplePlayer(std::shared_ptr<World> world) : Mob(world), Inventory(36) {
    m_header = ObjectHeader::PLAYER;
}

const char* SimplePlayer::getAnimationName() {
    // clang-format off
    switch(m_animType) {
        case PLAYER_IDLE: return "PLAYER_IDLE";
        case PLAYER_MOVE: return "PLAYER_MOVE";
        case PLAYER_SNEAK: return "PLAYER_SNEAK";
        case PLAYER_JUMP: return "PLAYER_JUMP";
        case PLAYER_HIT: return "PLAYER_HIT";
        case PLAYER_HURT: return "PLAYER_HURT";
        case PLAYER_SIT: return "PLAYER_SIT";
        case PLAYER_CART: return "PLAYER_CART";
        default: return "UNKNOWN";
    }
    //clang-format on
}

Vec2i SimplePlayer::getAnimLimit(AnimationType type) {
    switch (type) {
        case PLAYER_MOVE: return {1, 5};
        case PLAYER_SNEAK: return {6, 7};
        case PLAYER_HIT: return {9, 13};
        default: return {static_cast<int>(type), static_cast<int>(type)};
    }
}

void SimplePlayer::setAnimation(AnimationType type) {
    auto limit = getAnimLimit(type);
    m_animType = type;
    m_animFrame = animationClamp(m_animFrame, limit.x, limit.y);
}

DataStream SimplePlayer::serialize() {
    auto ret = Mob::serialize();

    ret.add(m_id);
    ret.add(m_animFrame);
    ret.add<uint8_t>('I');

    for (auto i = 0; i < m_inventory.size(); i++) {
        if (m_inventory[i]) {
            ret.add<uint8_t>(i);
        }
    }

    return ret;
}

bool SimplePlayer::deserialize(DataStream& stream) {
    // std::lock_guard<std::mutex> guard(m_mutex);
    if(!Mob::deserialize(stream)) return false;

    m_id = stream.get<PlayerID>(0);
    m_animFrame = stream.get<uint8_t>(0);

    return true;
}