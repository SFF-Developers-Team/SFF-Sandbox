#include <Entity.hpp>
#include <Block.hpp>
#include <World.hpp>

Entity::Entity(std::shared_ptr<World> world) : m_world(world), m_hitbox(0.0f, 0.0f, 0.8f, 1.65f) {
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
    // Store previous position
    m_prevX = m_hitbox.x;
    m_prevY = m_hitbox.y;
}

void Entity::move(float x, float y) {
    auto prevX = x;
    auto prevY = y;
    auto hitboxes = m_world->getHitboxes(m_hitbox.expand(x, y));

    // Check for X collision
    for (auto& hitbox : hitboxes) {
        x = hitbox.clipXCollide(m_hitbox, x);
    }
    m_hitbox.move(x, 0.0f);

    // Check for Y collision
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

void Entity::resetPosition() {
    m_hitbox.x = 0.0f;
    m_hitbox.y = -3.0f;
}

ByteVector Entity::serialize() {
    SerializedObject::serialize();

    add(m_hitbox.x);
    add(m_hitbox.y);
    add(m_direction);

    return bytes();
}

size_t Entity::deserialize(ByteVector const& bytes) {
    SerializedObject::deserialize(bytes);

    m_hitbox.x = get<float>();
    m_hitbox.y = get<float>();
    m_direction = get<Direction>();

    return m_offset;
}