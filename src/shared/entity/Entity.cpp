#include <Entity.hpp>
#include <Block.hpp>
#include <World.hpp>

Entity::Entity(World* world) : m_world(world) {
    m_header = ENTITY;
}

Entity::Entity(World* world, Vec2f position, bool enablePhysics) : m_world(world), m_enabledPhysics(enablePhysics) {
    m_header = ENTITY;
    m_hitbox.x = position.x;
    m_hitbox.y = position.y;
}

void Entity::updatePhysics() {
    auto delta = 0.017f;
    bool hitFloor = false;
    bool hitWall = false;
    bool hitCeil = false;

    for(auto& bh : m_world->getHitboxes(m_hitbox)) {
        // hit the floor
        if((bh.x < m_hitbox.x || bh.x < m_hitbox.x + m_hitbox.width) && 
            bh.x + bh.width > m_hitbox.x && 
            m_hitbox.y + m_hitbox.height <= bh.y && 
            m_hitbox.y + m_hitbox.height + m_speed.y * delta >= bh.y
        ) {
            m_hitbox.y = bh.y - m_hitbox.height;
            m_speed.y = 0.0f;
            hitFloor = true;
        }

        // hit the ceil
        if((bh.x < m_hitbox.x || bh.x < m_hitbox.x + m_hitbox.width) && 
            bh.x + bh.width > m_hitbox.x && 
            bh.y + bh.height <= m_hitbox.y && 
            bh.y + bh.height >= m_hitbox.y + m_speed.y * delta
        ) {
            m_hitbox.y = bh.y + bh.height;
            m_speed.y = abs(m_speed.y / 2);
            hitCeil = true;
        }

        // hit right wall
        if (((bh.y > m_hitbox.y && bh.y < m_hitbox.y + m_hitbox.height) ||
            (bh.y + bh.height > m_hitbox.y && bh.y + bh.height < m_hitbox.y + m_hitbox.height)) &&
            m_hitbox.x + m_hitbox.width + m_speed.x * delta >= bh.x &&
            m_hitbox.x + m_hitbox.width <= bh.x + bh.width

        ) {
            m_hitbox.x = bh.x - m_hitbox.width;          
            m_speed.x = 0.0f;
            hitWall = true;
        }

        // hit left wall
        if (((bh.y > m_hitbox.y && bh.y < m_hitbox.y + m_hitbox.height) ||
            (bh.y + bh.height > m_hitbox.y && bh.y + bh.height < m_hitbox.y + m_hitbox.height)) &&
            m_hitbox.x + m_speed.x * delta <= bh.x + bh.width &&
            m_hitbox.x >= bh.x

        ) {
            m_hitbox.x = bh.x + bh.width;          
            m_speed.x = 0.0f;
            hitWall = true;
        }
    }

    if(!hitWall) {
        m_hitbox.x += m_speed.x * delta;
    }

    if(!hitFloor) {
        m_hitbox.y += m_speed.y * delta;
        m_speed.y += m_gravitation * delta;
    }

    processPhysics(hitWall, hitFloor, hitCeil);
}

void Entity::update() {
    if(m_enabledPhysics) {
        updatePhysics();
    }
}

ByteVector& Entity::serialize() {
    SerializedObject::serialize();

    addBytes(m_hitbox);

    return m_bytes;
}

int Entity::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);

    m_hitbox = getBytes<Rectf>();

    return m_offset;
}