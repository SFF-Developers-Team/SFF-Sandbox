#include <entity/Mob.hpp>
#include <world/World.hpp>

Mob::Mob(std::shared_ptr<World> world) : Entity(world), m_health(getMaxHealth()), m_prevOnGround(false) {
    for(auto i = 0; i < world->getHeight(); i++) {
        auto block = world->getBlock(m_hitbox.x, i, 1);

        if(block && block->getHitbox().width > 0) {
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
        // m_lastHurtTime = GetTime();
    }

    m_prevOnGround = m_onGround;
}