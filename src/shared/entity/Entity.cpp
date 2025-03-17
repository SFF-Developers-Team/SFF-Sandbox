#include <entity/Entity.hpp>
#include <world/Block.hpp>
#include <world/World.hpp>

Entity::Entity() : Serializable(ENTITY), m_hitbox(0.0f, 0.0f, 0.8f, 1.65f), m_speed({0.f, 0.f}), m_direction(RIGHT) {}

void Entity::setPosition(Vec2f pos) {
    m_hitbox.x = pos.x;
    m_hitbox.y = pos.y;
}

void Entity::setSize(Vec2f size) {
    m_hitbox.width = size.x;
    m_hitbox.height = size.y;
}

void Entity::move(std::vector<Hitbox>& env, float xa, float ya) {
    auto prevX = xa;
    auto prevY = ya;

    // Check for X collision
    for (auto& hitbox : env) {
        xa = hitbox.clipXCollide(m_hitbox, xa);
    }
    m_hitbox.move(xa, 0.0f);

    // Check for Y collision
    for (auto& hitbox : env) {
        ya = hitbox.clipYCollide(m_hitbox, ya);
    }
    m_hitbox.move(0.0f, ya);

    // Update on ground state
    m_onGround = prevY != ya && prevY > 0.f;

    // Stop motion on collision
    if (prevX != xa) m_speed.x = 0.f;
    if (prevY != ya) m_speed.y = 0.f;
}

DataStream Entity::serialize() {
    auto ret = Serializable::serialize();

    ret.add(m_hitbox.x);
    ret.add(m_hitbox.y);
    ret.add(m_direction);

    return ret;
}

bool Entity::deserialize(DataStream& stream) {
    if(!Serializable::deserialize(stream)) return false;

    m_hitbox.x = stream.get<float>();
    m_hitbox.y = stream.get<float>();
    m_direction = stream.get<Direction>();

    return true;
}