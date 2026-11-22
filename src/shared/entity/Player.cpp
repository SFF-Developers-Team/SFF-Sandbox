#include "Types.hpp"
#include <entity/Player.hpp>
#include <entity/Entity.hpp>
#include <world/World.hpp>

Player::Player(World& world) : Mob(world), Inventory(36), m_id(0), m_gamemode(GAMEMODE_SURVIVAL), m_fly(false), m_breakingTile(false) {
    m_header = ObjectHeader::PLAYER;

    // clang-format off
    Col3u const colors[] = {
        {255, 255, 255}, 
        {255, 41, 55}, 
        {255, 128, 0},   
        {255, 255, 0},   
        {0, 255, 0},     
        {0, 255, 255}, 
        {0, 0, 255},
        {128, 0, 255},   
        {255, 0, 255}, 
        {255, 128, 255}, 
        {255, 128, 255}, 
        {128, 128, 128}, 
        {0, 0, 0},     
        {128, 64, 0}
    };
    // clang-format on

    if(m_gamemode == GAMEMODE_CREATIVE) {
        for(int i = 1; i < 20; i++) {
            if(i == (int)TileID::BEDROCK) {
                continue;
            }

            addItem(std::make_unique<Item>(static_cast<ItemID>(i)), 64);
        }

        // for (auto& col : colors) {
        //     auto wool = std::make_shared<InventoryItem>(WOOL, 64);
        //     wool->setTag(TagID::TAG_COLOR, col);
        //     addItem(wool);
        // }
    }
}

const char* Player::getAnimationName() {
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

Vec2i Player::getAnimLimit(AnimationType type) {
    switch (type) {
        case PLAYER_MOVE: return {1, 5};
        case PLAYER_SNEAK: return {6, 7};
        case PLAYER_HIT: return {9, 13};
        default: return {static_cast<int>(type), static_cast<int>(type)};
    }
}

void Player::setAnimation(AnimationType type) {
    auto limit = getAnimLimit(type);
    m_animType = type;
    if (limit.x != limit.y) {
        m_animFrame = animationClamp(m_animFrame, limit.x, limit.y);
    }
}

DataStream Player::serialize() {
    auto ret = Mob::serialize();

    ret.add(m_id);
    ret.add(m_animFrame);
    ret.add<uint8_t>('I');

    return ret;
}

bool Player::deserialize(DataStream& stream) {
    // std::lock_guard<std::mutex> guard(m_mutex);
    if(!Mob::deserialize(stream)) return false;

    m_id = stream.get<PlayerID>(0);
    m_animFrame = stream.get<uint8_t>(0);

    return true;
}

bool Player::canAccessTile(TilePosition target) {
    Vec2f playerPos{m_hitbox.x, m_hitbox.y};
    return playerPos.distance(Vec2f {static_cast<float>(target.x), static_cast<float>(target.y)}) <= 4.f;
}

bool Player::canDestroyTile(World& world, TilePosition target) {
    if (world.isOutOfBound(target)) {
        return false;
    }

    auto tile = world.getTile(target);

    if (tile == TileID::AIR || tile == TileID::BEDROCK) return false;

    return true;
}

bool Player::canPlaceTile(World& world, TilePosition target) {
    if (world.isOutOfBound(target)) {
        return false;
    }

    bool tileAround = (
        TileID::AIR != world.getTile({target.x - 1, target.y, target.layer}) || 
        TileID::AIR != world.getTile({target.x + 1, target.y, target.layer}) ||
        TileID::AIR != world.getTile({target.x, target.y - 1, target.layer}) || 
        TileID::AIR != world.getTile({target.x, target.y + 1, target.layer}) ||
        TileID::AIR != world.getTile({target.x, target.y, !target.layer})
    );

    return world.getTile(target) == TileID::AIR && tileAround;
}

void Player::setGamemode(GameMode gamemode) {
    if(gamemode != GAMEMODE_CREATIVE) {
        m_fly = false;
    }

    m_gamemode = gamemode;
}

void Player::toggleFly() {
    if(m_gamemode == GAMEMODE_CREATIVE) {
        m_fly = !m_fly;
    }
}

void Player::onTick(World& world) {
    if (m_onGround) {
        m_coyotTime = 7;
    } else if (m_coyotTime > 0) {
        m_coyotTime--;
    }

    Mob::onTick(world);
}