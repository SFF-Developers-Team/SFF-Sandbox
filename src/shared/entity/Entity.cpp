#include <Entity.hpp>
#include <Block.hpp>
#include <World.hpp>

Entity::Entity(World* world) : m_world(world), m_hitbox(0.0f, 0.0f, 0.8f, 1.65f) {
    m_header = ENTITY;
}

void Entity::setPosition(Vec2f pos) {
    m_hitbox.x = pos.x;
    m_hitbox.y = pos.y;
}

void Entity::setSize(Vec2f size) {
    m_hitbox.width = size.x;
    m_hitbox.height = size.y;
}

void Entity::turn(Direction dir) {
    m_direction = dir;
}

void Entity::onTick() {
    m_prevX = m_hitbox.x;
    m_prevY = m_hitbox.y;
}

void Entity::move(float x, float y) {
    auto prevX = x;
    auto prevY = y;
    auto hitboxes = m_world->getHitboxes(m_hitbox.expand(x, y));

    for (auto& hitbox : hitboxes) {
        x = hitbox.clipXCollide(m_hitbox, x);
    }
    m_hitbox.move(x, 0.0f);

    for (auto& hitbox : hitboxes) {
        y = hitbox.clipYCollide(m_hitbox, y);
    }
    m_hitbox.move(0.0f, y);

    // Update on ground state
    m_onGround = prevY != y && prevY > 0.f;

    // Stop motion on collision
    if (prevX != x) m_speedX = 0.f;
    if (prevY != y) m_speedY = 0.f;
}

void Entity::moveRelative(float x, float speed) {
    if(x < 0.01f && x > -0.01f) return;
    float distance = speed / x;
    x *= distance;
    m_speedX = x * (m_direction == LEFT ? -1 : 1);
}

void Entity::resetPosition() {
    m_hitbox.x = rand() % m_world->getWidth();
    m_hitbox.y = -3.0f;
}

ByteVector& Entity::serialize() {
    SerializedObject::serialize();

    addBytes(m_hitbox.x);
    addBytes(m_hitbox.y);
    addBytes(m_direction);

    return m_bytes;
}

int Entity::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);

    m_hitbox.x = getBytes<float>();
    m_hitbox.y = getBytes<float>();
    m_direction = getBytes<Direction>();

    return m_offset;
}