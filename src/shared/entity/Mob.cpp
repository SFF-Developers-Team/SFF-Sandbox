#include <entity/Mob.hpp>
#include <world/World.hpp>

Mob::Mob(std::shared_ptr<World> world) : Entity(), m_health(getMaxHealth()), m_prevOnGround(false), m_lastHurtTime(0.f) {
    for(auto i = 0; i < world->getHeight(); i++) {
        auto hitbox = world->getBlockHitbox({(int)m_hitbox.x, i, 1});

        if(hitbox.height > 0) {
            m_hitbox.y = i - 2;
            break;
        }
    }

    m_fallY = m_hitbox.y;
}

void Mob::onTick() {
    if (m_prevOnGround && !m_onGround) {
        m_fallY = m_hitbox.y;
    }

    float fallenBlocks = m_hitbox.y - m_fallY;

    if(!m_prevOnGround && m_onGround && fallenBlocks > 4.f) {
        m_health -= floor(fallenBlocks / 4.f);
        // m_lastHurtTime = m_world->getTime();
    }

    m_prevOnGround = m_onGround;
}

DataStream Mob::serialize() {
    auto ret = Entity::serialize();
    
    ret.add(m_health);
    ret.add(m_lastHurtTime);
    ret.add(m_fallY);

    return ret;
}

bool Mob::deserialize(DataStream& stream) {
    if(!Entity::deserialize(stream)) return false;

    m_health = stream.get<int>(20);
    m_lastHurtTime = stream.get<uint64_t>(0);
    m_fallY = stream.get<float>();

    return true;
}