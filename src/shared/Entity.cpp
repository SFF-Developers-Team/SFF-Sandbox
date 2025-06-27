#include "Entity.hpp"

Entity::Entity(World& world) : Box(), m_world(world) {}

void Entity::OnTick() {
    m_speed.y += 0.02f;

    MoveInWorld(m_speed.x, m_speed.y);

    m_speed.x *= 0.91f;
    m_speed.y *= 0.98f;
    
    if (m_onGround) {
        m_speed.x *= 0.7f;
        m_speed.y *= 0.7f;
    }
}

void Entity::MoveInWorld(float x, float y) {
    auto prevX = x;
    auto prevY = y;
    auto hitboxes = m_world.GetBlocksAround(Expand(x, y));

    // Check for X collision
    for (auto& hitbox : hitboxes) {
        x = hitbox.ClipCollideX(*this, x);
    }
    this->x += x;

    // Check for Y collision
    for (auto& hitbox : hitboxes) {
        y = hitbox.ClipCollideY(*this, y);
    }
    this->y += y;

    // Update on ground state
    m_onGround = prevY != y && prevY > 0.f;

    // Stop motion on collision
    if (prevX != x) m_speed.x = 0.f;
    if (prevY != y) m_speed.y = 0.f;
}